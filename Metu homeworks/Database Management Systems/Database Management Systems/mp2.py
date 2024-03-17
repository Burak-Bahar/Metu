from seller import Seller
import datetime as dt
import psycopg2
import configparser as cp
from messages import *
import string
import random

"""
    Splits given command string by spaces and trims each token.
    Returns token list.
"""


def tokenize_command(command):
    tokens = command.split(" ")
    return [t.strip() for t in tokens]


def order_id_generator():
    o_id = ''.join(random.choices(string.ascii_lowercase +
                                string.digits, k=32))
    return o_id


class Mp2Client:
    def __init__(self, config_filename):
        cfg = cp.ConfigParser()
        cfg.read(config_filename)
        self.db_conn_params = cfg["postgresql"]
        self.conn = None

    """
        Connects to PostgreSQL database and returns connection object.
    """

    def connect(self):
        self.conn = psycopg2.connect(**self.db_conn_params)
        self.conn.autocommit = False

    """
        Disconnects from PostgreSQL database.
    """

    def disconnect(self):
        self.conn.close()

    """
        Prints list of available commands of the software.
    """

    def help(self):
        # prints the choices for commands and parameters
        print("\n*** Please enter one of the following commands ***")
        print("> help")
        print("> sign_up <seller_id> <subscriber_key> <zip_code> <city> <state> <plan_id>")
        print("> sign_in <seller_id> <subscriber_key>")
        print("> sign_out")
        print("> show_plans")
        print("> show_subscription")
        print("> change_stock <product_id> <add or remove> <amount>")
        print("> show_quota")
        print("> subscribe <plan_id>")
        print("> ship <product_id_1> <product_id_2> <product_id_3> ... <product_id_n>")
        print("> calc_gross")
        print("> show_cart <customer_id>")
        print("> change_cart <customer_id> <product_id> <seller_id> <add or remove> <amount>")
        print("> purchase_cart <customer_id>")
        print("> quit")

    """
        Saves seller with given details.
        - Return type is a tuple, 1st element is a boolean and 2nd element is the response message from messages.py.
        - If the operation is successful, commit changes and return tuple (True, CMD_EXECUTION_SUCCESS).
        - If any exception occurs; rollback, do nothing on the database and return tuple (False, CMD_EXECUTION_FAILED).
    """

    def sign_up(self, seller_id, sub_key, zip, city, state, plan_id):

        try:
            cursor = self.conn.cursor()
            query = "SELECT seller_id " \
                    "FROM sellers " \
                    "WHERE seller_id = '%s'; " % seller_id

            cursor.execute(query)
            if cursor.fetchone() is not None:
                # Seller exists
                return False, CMD_EXECUTION_FAILED

            seller_query = "insert into sellers(seller_id, seller_zip_code_prefix, seller_city, seller_state) " \
                           "VALUES('%s','%s','%s','%s'); " % (seller_id, zip, city, state)
            subs_query = "insert into seller_subscription(seller_id, subscriber_key, session_count, plan_id) " \
                         "VALUES ('%s', '%s', %s, %s); " % (seller_id, sub_key, 0, plan_id)
            cursor.execute(seller_query)
            cursor.execute(subs_query)
            self.conn.commit()

            return True, CMD_EXECUTION_SUCCESS

        except Exception as e:
            # if there is any problem in the above
            self.conn.rollback()
            return False, CMD_EXECUTION_FAILED

    """
        Retrieves seller information if seller_id and subscriber_key is correct and seller's session_count < max_parallel_sessions.
        - Return type is a tuple, 1st element is a seller object and 2nd element is the response message from messages.py.
        - If seller_id or subscriber_key is wrong, return tuple (None, USER_SIGNIN_FAILED).
        - If session_count < max_parallel_sessions, commit changes (increment session_count) and return tuple (seller, CMD_EXECUTION_SUCCESS).
        - If session_count >= max_parallel_sessions, return tuple (None, USER_ALL_SESSIONS_ARE_USED).
        - If any exception occurs; rollback, do nothing on the database and return tuple (None, USER_SIGNIN_FAILED).
    """
    # Fail in wrong sign in credentials
    # Fail in out of session
    def sign_in(self, seller_id, sub_key):
        # TODO: implement this function
        try:
            cursor = self.conn.cursor()
            seller_query = "SELECT * " \
                           "FROM seller_subscription " \
                           "WHERE seller_id = '%s' and subscriber_key = '%s'; " % (seller_id, sub_key)
            cursor.execute(seller_query)
            seller_sub = cursor.fetchone()
            if seller_sub is None:
                return None, USER_SIGNIN_FAILED

            subs_query = "SELECT max_parallel_sessions " \
                         "From subscription_plans " \
                         "where plan_id = %s " % seller_sub[3]
            cursor.execute(subs_query)
            max_session = cursor.fetchone()[0]
            if seller_sub[2] < max_session:
                cursor.execute("update seller_subscription set session_count = session_count+1 "
                               "WHERE seller_id = '%s' and subscriber_key = '%s'; " % (seller_id, sub_key))

                self.conn.commit()
                return Seller(seller_sub[0], seller_sub[2] + 1, seller_sub[3]), CMD_EXECUTION_SUCCESS
            else:
                return None, USER_ALL_SESSIONS_ARE_USED
        except Exception as e:
            self.conn.rollback()
            return None, USER_SIGNIN_FAILED

    """
        Signs out from given seller's account.
        - Return type is a tuple, 1st element is a boolean and 2nd element is the response message from messages.py.
        - Decrement session_count of the seller in the database.
        - If the operation is successful, commit changes and return tuple (True, CMD_EXECUTION_SUCCESS).
        - If any exception occurs; rollback, do nothing on the database and return tuple (False, CMD_EXECUTION_FAILED).
    """

    def sign_out(self, seller):
        # TODO: implement this function
        try:
            cursor = self.conn.cursor()

            cursor.execute("update seller_subscription set session_count = "
                           "(case when session_count > 0 then (session_count - 1) else 0 end)"
                           "WHERE seller_id = '%s'; " % seller.seller_id)
            self.conn.commit()
            return True, CMD_EXECUTION_SUCCESS
        except Exception as e:
            self.conn.rollback()
            return False, CMD_EXECUTION_FAILED

    """
        Quits from program.
        - Return type is a tuple, 1st element is a boolean and 2nd element is the response message from messages.py.
        - Remember to sign authenticated user out first.
        - If the operation is successful, commit changes and return tuple (True, CMD_EXECUTION_SUCCESS).
        - If any exception occurs; rollback, do nothing on the database and return tuple (False, CMD_EXECUTION_FAILED).
    """

    def quit(self, seller):
        # TODO: implement this function
        if seller is not None:

            TF, massage = self.sign_out(seller=seller)
            if TF:
                return True, CMD_EXECUTION_SUCCESS
            else:
                return False, CMD_EXECUTION_FAILED

        return True, CMD_EXECUTION_SUCCESS

    """
        Retrieves all available plans and prints them.
        - Return type is a tuple, 1st element is a boolean and 2nd element is the response message from messages.py.
        - If the operation is successful; print available plans and return tuple (True, CMD_EXECUTION_SUCCESS).
        - If any exception occurs; return tuple (False, CMD_EXECUTION_FAILED).
        
        Output should be like:
        #|Name|Max Sessions|Max Stocks Per Product
        1|Basic|2|4
        2|Advanced|4|8
        3|Premium|6|12
    """

    def show_plans(self):
        # TODO: implement this function
        try:
            cursor = self.conn.cursor()
            cursor.execute("select * from subscription_plans order by plan_id asc")
            plans = cursor.fetchall()
            cursor.close()
            print('#|Name|Max Sessions|Max Stocks Per Product')
            for plan in plans:
                print('%s|%s|%s|%s' % (plan[0], plan[1], plan[2], plan[3]))
            return True, CMD_EXECUTION_SUCCESS
        except Exception:
            return False, CMD_EXECUTION_FAILED

    def show_subscription(self, seller):
        # TODO: implement this function
        try:
            cursor = self.conn.cursor()
            cursor.execute("select P.plan_id, plan_name, max_parallel_sessions, max_stock_per_product "
                           "from subscription_plans P , seller_subscription S "
                           "where S.seller_id = '%s' and S.plan_id = P.plan_id ;" % seller.seller_id)
            plan = cursor.fetchone()
            cursor.close()
            print('#|Name|Max Sessions|Max Stocks Per Product')
            print('%s|%s|%s|%s' % (plan[0], plan[1], plan[2], plan[3]))
            return True, CMD_EXECUTION_SUCCESS
        except Exception as e:
            return False, CMD_EXECUTION_FAILED

    """
        Change stock count of a product.
        - Return type is a tuple, 1st element is a seller object and 2nd element is the response message from messages.py.
        - If target product does not exist on the database, return tuple (False, PRODUCT_NOT_FOUND).
        - If target stock count > current plan's max_stock_per_product, return tuple (False, QUOTA_LIMIT_REACHED).
        - If the operation is successful, commit changes and return tuple (seller, CMD_EXECUTION_SUCCESS).
        - If any exception occurs; rollback, do nothing on the database and return tuple (False, CMD_EXECUTION_FAILED).
    """

    # check if product_id exists in product, table if not fail
    # if seller doesn't have any stock of given product check :
        # If adding stock, add the product to seller stocks (granted that amount is in range between 0 and max stock per product)
        # if removing stock there is not any product in seller stock with giving product_id, so fail
    # else if product is in seller stocks
        # if the change results in the range between 0 and max stock per product, update seller stocks
        # elif the change result in 0, then after modification there is no stock remains in seller stock, so delete the row
        # else it fails
    def change_stock(self, seller, product_id, change_amount):
        # TODO: implement this function
        try:
            cursor = self.conn.cursor()
            cursor.execute("select product_id from products where product_id = '%s';" % product_id)
            product = cursor.fetchone()

            if product is None:
                return False, PRODUCT_NOT_FOUND
            else:
                cursor.execute("select max_stock_per_product from subscription_plans where plan_id ='%s';" % seller.plan_id)
                max_stock = cursor.fetchone()[0]

                cursor.execute("select * from seller_stocks where product_id = '%s' and seller_id = '%s';" % (product_id,seller.seller_id)) # check if sellers sells it
                exist = cursor.fetchone()

                if exist is None:

                    if change_amount > 0:

                        if change_amount < max_stock:
                            cursor.execute("insert into seller_stocks(seller_id, product_id, stock_count) "
                                           "VALUES ('%s','%s',%s);" % (seller.seller_id,product_id,change_amount))
                            self.conn.commit()
                            return seller, CMD_EXECUTION_SUCCESS
                        else:
                            cursor.close()
                            return False, STOCK_UPDATE_FAILURE
                    else:
                        cursor.close()
                        return False, STOCK_UPDATE_FAILURE
                else:
                    if max_stock >= exist[2] + change_amount > 0:
                        cursor.execute("update seller_stocks "
                                       "set stock_count = stock_count + '%s' "
                                       "where seller_id = '%s' and product_id = '%s';" % (
                                           change_amount, seller.seller_id, product_id))

                    elif exist[2] + change_amount == 0:
                        cursor.execute("delete from seller_stocks where product_id = '%s' and seller_id = '%s';" % (product_id, seller.seller_id))
                    else:
                        cursor.close()
                        if max_stock <= exist[2] + change_amount:
                            return False, QUOTA_LIMIT_REACHED
                        else:
                            return False, STOCK_UPDATE_FAILURE

                    self.conn.commit()
                    return seller, CMD_EXECUTION_SUCCESS
        except Exception as e:

            self.conn.rollback()
            return False, CMD_EXECUTION_FAILED

    """
        Retrieves authenticated seller's remaining quota for stocks and prints it. 
        - Return type is a tuple, 1st element is a boolean and 2nd element is the response message from messages.py.
        - If the operation is successful; print the authenticated seller's quota and return tuple (True, CMD_EXECUTION_SUCCESS).
        - If any exception occurs; return tuple (False, CMD_EXECUTION_FAILED).

        If the seller is subscribed to a plan with max_stock_per_product = 12 and
        the current stock for product 92bf5d2084dfbcb57d9db7838bac5cd0 is 10, then output should be like:
        
        Product Id|Remaining Quota
        92bf5d2084dfbcb57d9db7838bac5cd0|2

        If the seller does not have a stock, print 'Quota limit is not activated yet.'
    """

    def show_quota(self, seller):
        # TODO: implement this function
        try:
            cursor = self.conn.cursor()
            cursor.execute("select product_id, max_stock_per_product - stock.stock_count as rem_quota "
                           "from subscription_plans plan , seller_subscription sub, seller_stocks stock "
                           "where sub.seller_id = '%s' and sub.seller_id = stock.seller_id and sub.plan_id = plan.plan_id;" % seller.seller_id)
            results = cursor.fetchall()
            cursor.close()
            if not results:
                return False, QUOTA_INACTIVE
            else:
                print("Product Id|Remaining Quota")
                for tmp in results:
                    print("%s|%s" % (tmp[0], tmp[1]))
                return True, CMD_EXECUTION_SUCCESS
        except Exception:
            return False, CMD_EXECUTION_FAILED

    """
        Subscribe authenticated seller to new plan.
        - Return type is a tuple, 1st element is a seller object and 2nd element is the response message from messages.py.
        - If target plan does not exist on the database, return tuple (None, SUBSCRIBE_PLAN_NOT_FOUND).
        - If the new plan's max_parallel_sessions < current plan's max_parallel_sessions, return tuple (None, SUBSCRIBE_MAX_PARALLEL_SESSIONS_UNAVAILABLE).
        - If the operation is successful, commit changes and return tuple (seller, CMD_EXECUTION_SUCCESS).
        - If any exception occurs; rollback, do nothing on the database and return tuple (None, CMD_EXECUTION_FAILED).
    """

    def subscribe(self, seller, plan_id):
        # TODO: implement this function
        try:
            cursor = self.conn.cursor()
            cursor.execute("select max_parallel_sessions from subscription_plans where plan_id = '%s';" % plan_id)
            plan = cursor.fetchone()
            if plan is None:
                return None, SUBSCRIBE_PLAN_NOT_FOUND
            else:
                cursor.execute("select max_parallel_sessions from subscription_plans plan "
                               "where plan.plan_id = %s ; " % seller.plan_id)
                cur_max = cursor.fetchone()
                if plan >= cur_max:
                    cursor.execute(
                        "update seller_subscription set plan_id = %s where seller_id = '%s';" % (
                            plan_id, seller.seller_id))
                    self.conn.commit()
                    seller.plan_id = plan_id
                    return seller, CMD_EXECUTION_SUCCESS
                else:
                    return None, SUBSCRIBE_MAX_PARALLEL_SESSIONS_UNAVAILABLE
        except Exception:
            self.conn.rollback()
            return None, CMD_EXECUTION_FAILED

    """
        Change stock amounts for multiple distinct products.
        - Return type is a tuple, 1st element is a boolean and 2nd element is the response message from messages.py.
        - If everything is OK and the operation is successful, return (True, CMD_EXECUTION_SUCCESS).
        - If the operation is successful, commit changes and return tuple (True, CMD_EXECUTION_SUCCESS).
        - If any one of the product ids is incorrect; rollback, do nothing on the database and return tuple (False, CMD_EXECUTION_FAILED).
        - If any one of the products is not in the stock; rollback, do nothing on the database and return tuple (False, CMD_EXECUTION_FAILED).
        - If any exception occurs; rollback, do nothing on the database and return tuple (False, CMD_EXECUTION_FAILED).
    """

    # there is no indicator how of the stock the sellers ships, so I am just removing 1 stock per product
    def ship(self, seller, product_ids):
        # TODO: implement this function
        try:
            cursor = self.conn.cursor()
            for pp in product_ids:

                # some products don't exist
                cursor.execute("select product_id from products where product_id = '%s';" % pp)
                tmp = cursor.fetchone()
                if tmp is None:
                    return False, CMD_EXECUTION_FAILED
                # or product is not in sellers stock

                cursor.execute(
                    "select stock_count from seller_stocks where seller_id = '%s' and product_id = '%s'; " % (
                        seller.seller_id, pp))
                tmp = cursor.fetchone()
                if tmp is None:
                    return False, CMD_EXECUTION_FAILED
            # all products of a seller

            for pp in product_ids:
                cursor.execute(
                    "update seller_stocks set stock_count = stock_count-1 where seller_id = '%s' and product_id = '%s'; " % (
                        seller.seller_id, pp))
                cursor.execute("delete from seller_stocks where seller_id = '%s' and product_id = '%s' and stock_count = 0; " % (seller.seller_id, pp))
            self.conn.commit()
            return True, CMD_EXECUTION_SUCCESS
        except Exception:
            self.conn.rollback()
            return False, CMD_EXECUTION_FAILED

    """
        Retrieves the gross income per product category for every month.
        - Return type is a tuple, 1st element is a boolean and 2nd element is the response message from messages.py.
        - If the operation is successful; print the results and return tuple (True, CMD_EXECUTION_SUCCESS).
        - If any exception occurs; return tuple (False, CMD_EXECUTION_FAILED).
        
        Output should be like:
        Gross Income|Year|Month
        123.45|2018|1
        67.8|2018|2
    """
    # I didn't know how calculate gross incom, so I just summed the prices
    def calc_gross(self, seller):
        # TODO: implement this function
        try:
            # There was no indication of gross income calculation so, I just sum the prices
            cursor = self.conn.cursor()
            cursor.execute(
                "select EXTRACT(YEAR FROM order_purchase_timestamp) as Year, EXTRACT(MONTH FROM order_purchase_timestamp) as Month, sum(price) "
                "from orders oo, order_items oi "
                "where oi.seller_id= '%s' and oo.order_id = oi.order_id "
                "group by EXTRACT(YEAR FROM order_purchase_timestamp), EXTRACT(MONTH FROM order_purchase_timestamp);" % seller.seller_id)
            gross = cursor.fetchall()
            cursor.close()
            print("Gross Income|Year|Month")
            for income in gross:
                print('%s,%s,%s' % (income[2], income[0], income[1]))
            return True, CMD_EXECUTION_SUCCESS
        except Exception:
            return False, CMD_EXECUTION_FAILED

    """
        Retrieves items on the customer's shopping cart
        - Return type is a tuple, 1st element is a boolean and 2nd element is the response message from messages.py.
        - If the operation is successful; print items on the cart and return tuple (True, CMD_EXECUTION_SUCCESS).
        - If any exception occurs; return tuple (False, CMD_EXECUTION_FAILED).
        
        Output should be like:
        Seller Id|Product Id|Amount
        dd7ddc04e1b6c2c614352b383efe2d36|e5f2d52b802189ee658865ca93d83a8f|2
        5b51032eddd242adc84c38acab88f23d|c777355d18b72b67abbeef9df44fd0fd|3
        df560393f3a51e74553ab94004ba5c87|ac6c3623068f30de03045865e4e10089|1
    """

    def show_cart(self, customer_id):
        # TODO: implement this function
        try:
            cursor = self.conn.cursor()
            cursor.execute("select * from customer_carts where customer_id = '%s'; " % customer_id)
            cart = cursor.fetchall()
            cursor.close()
            print('Seller Id|Product Id|Amount')
            for item in cart:
                print('%s|%s|%s' % (item[1], item[2], item[3]))
            return True, CMD_EXECUTION_SUCCESS
        except Exception as e:
            return False, CMD_EXECUTION_FAILED

    """
        Change count of items in shopping cart
        - Return type is a tuple, 1st element is a seller object and 2nd element is the response message from messages.py.
        - If customer does not exist on the database, return tuple (False, CUSTOMER_NOT_FOUND).
        - If target product does not exist on the database, return tuple (False, PRODUCT_NOT_FOUND).
        - If the operation is successful, commit changes and return tuple (True, CMD_EXECUTION_SUCCESS).
        - If any exception occurs; rollback, do nothing on the database and return tuple (False, CMD_EXECUTION_FAILED).
        - Consider stocks of sellers when you add items to the cart.
    """

    # if custumer_id can't be found in customers table then fail
    # if product_id can't be found in products table then fail
    # check if seller sells the product, if not fail
    # check if the product is in cart
    # if product is not in customer_cart check:
        # If adding amount, add the product to customer_cart (granted that amount is in range between 0 and seller has the amount of product)
        # if removing amount there is not any product in customer_cart with giving product_id, so fail
    # else if product is in customer_cart:
        # if the change results in the range between 0 and sellers stock, update seller stocks
        # elif the change result in 0, then after modification there is no amount remains in customer_cart, so delete the row
        # else it fails

    def change_cart(self, customer_id, product_id, seller_id, change_amount):
        # TODO: implement this function
        try:
            cursor = self.conn.cursor()
            cursor.execute("select customer_id from customers where customer_id = '%s';" % customer_id)
            customer = cursor.fetchone()

            if customer is None:
                cursor.close()
                return False, CUSTOMER_NOT_FOUND

            cursor.execute("select product_id from products where product_id = '%s';" % product_id)
            product = cursor.fetchone()

            if product is None:
                cursor.close()
                return False, PRODUCT_NOT_FOUND

            # stock in seller
            cursor.execute("select * from seller_stocks where seller_id = '%s' and product_id = '%s';" % (
                seller_id, product_id))
            tmp2 = cursor.fetchone()
            if tmp2 is not None:
                seller_stock = tmp2[2]
            else:
                cursor.close()
                return False, STOCK_UNAVAILABLE

            # product in cart
            cursor.execute(
                "select * from customer_carts where customer_id = '%s' and seller_id ='%s' and product_id ='%s';" % (
                    customer_id, seller_id, product_id))
            tmp = cursor.fetchone()

            # if added  to cart
            if tmp is None:
                # if wanted stock is exist in the seller and added
                if seller_stock >= change_amount > 0:
                    # add
                    cursor.execute("insert into customer_carts(customer_id, seller_id, product_id, amount) VALUES ("
                                   "'%s','%s','%s',%s);" % (customer_id, seller_id, product_id, change_amount))

                elif seller_stock < change_amount:
                    cursor.close()
                    return False, STOCK_UNAVAILABLE
                else:
                    # error- change amount minus
                    cursor.close()
                    return False, CMD_EXECUTION_FAILED
            else:
                # exists in customer cart
                # add
                cus_cart_amount = tmp[3]
                if 0 < cus_cart_amount + change_amount <= seller_stock:
                    cursor.execute("update customer_carts set amount = amount+ %s where customer_id ='%s' and "
                                   "seller_id = '%s' and product_id = '%s';" % (
                                       change_amount, customer_id, seller_id, product_id))
                # delete
                elif cus_cart_amount + change_amount <= 0:
                    cursor.execute("delete from customer_carts where customer_id ='%s' and seller_id = '%s' and "
                                   "product_id = '%s';" % (customer_id, seller_id, product_id))
                # fail
                else:
                    cursor.close()
                    return False, STOCK_UNAVAILABLE

            self.conn.commit()
            return True, CMD_EXECUTION_SUCCESS
        except Exception as e:
            self.conn.rollback()
            return False, CMD_EXECUTION_FAILED

    """
        Purchases items on the cart
        - Return type is a tuple, 1st element is a boolean and 2nd element is the response message from messages.py.
        - If the operation is successful; return tuple (True, CMD_EXECUTION_SUCCESS).
        - If any exception occurs; return tuple (False, CMD_EXECUTION_FAILED).
        
        Actions:
        - Change stocks on seller_stocks table
        - Remove entries from customer_carts table
        - Add entries to order_items table
        - Add single entry to order table
    """


    # I will be using random string generator to generate order_id

    # check if customer is in customers table, if not fail
    # check if custumer has any items in their cart if not fail
    # for every item in cart do:
        # check whether seller has stock of the product if not fail
        # if seller has more stock then the customer wants update the seller_stock
        # if the amount in the stock and customers cart is equal to each other there will be no stock in the seller after check out, so remove the row from seller stocks
        # else if customers demand is bigger than the stock that seller has fail
        #
        # generate a order_id that is the length of 32 characters and take timestamp
        # insert to orders and order_items table
    # clear the customer cart
    def purchase_cart(self, customer_id):
        # TODO: implement this function
        try:
            cursor = self.conn.cursor()
            cursor.execute("select customer_id from customers where customer_id = '%s';" % customer_id)
            customer = cursor.fetchone()

            if customer is None:
                cursor.close()
                return False, CUSTOMER_NOT_FOUND

            cursor.execute("select * from customer_carts where customer_id = '%s';" % customer_id)
            cart_items = cursor.fetchone()
            if cart_items is None:
                # if there is no items in cart it is okay.
                cursor.close()
                return False, CMD_EXECUTION_FAILED

            for item in cart_items:

                # get seller stocks
                cursor.execute("select * from seller_stocks where seller_id = '%s' and product_id = '%s';" % (item[1], item[2]))
                seller_stocks = cursor.fetchone()
                if seller_stocks is None:
                    self.conn.rollback()
                    return False, STOCK_UNAVAILABLE

                if seller_stocks[2] > item[3]:  # seller_stock is bigger than what customer is demanding, update seller stocks
                    cursor.execute("update seller_stocks set stock_count = %s - %s where seller_id ='%s' and "
                                   "product_id ='%s';" % (seller_stocks[2], item[3], item[1], item[2]))

                elif seller_stocks[2] == item[3]:  # customers wants all product from the seller, delete row from seller stocks
                    cursor.execute("delete from seller_stocks where seller_id = '%s' and product_id ='%s';" % (item[1], item[2]))

                else:  # customer demands more than existing
                    self.conn.rollback()
                    return False, STOCK_UNAVAILABLE

                order_id = order_id_generator()
                cursor.execute("select order_id from orders where order_id = '%s';" % order_id)
                exist = cursor.fetchone()

                while exist is not None:
                    order_id = order_id_generator()
                    cursor.execute("select order_id from orders where order_id = '%s';" % order_id)
                    exist = cursor.fetchone()

                timestamp = dt.datetime.now()
                cursor.execute("insert into orders (order_id, customer_id, order_status, order_purchase_timestamp, order_approved_at, "
                               "order_delivered_carrier_date, order_delivered_customer_date, "
                               "order_estimated_delivery_date) "
                               "values (%s,%s,NULL,%s,NULL,NULL,NULL,NULL);" , (order_id, customer_id, timestamp,))  # add to orders
                cursor.execute("insert into order_items (order_id, order_item_id, product_id, seller_id, "
                               "shipping_limit_date, price, freight_value) "
                               "values ('%s',%s,'%s','%s',NULL,NULL,NULL);" % (order_id, item[3], item[2], item[1]))  # add to order_items
            cursor.execute("delete from customer_carts where customer_id = '%s';" % customer_id)  # delete customer_cart
            self.conn.commit()
            return True, CMD_EXECUTION_SUCCESS
        except Exception as e:

            self.conn.rollback()
            return False, CMD_EXECUTION_FAILED
