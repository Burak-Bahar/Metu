package ceng.ceng351.foodrecdb;

import java.sql.*;
import java.util.ArrayList;

public class FOODRECDB implements IFOODRECDB {

    private static Connection connection = null;


    @Override
    public void initialize(){

        String host = "momcorp.ceng.metu.edu.tr";
        String database = "db2380137";
        int port = 8080;
        String url = "jdbc:mysql://" + host + ":" + port + "/" + database;

        try {
            Class.forName("com.mysql.cj.jdbc.Driver");
            String user = "e2380137";
            String password = "b6_s_C9K39DJqTqS";
            connection =  DriverManager.getConnection(url, user, password);
        } catch (SQLException | ClassNotFoundException e) {
            e.printStackTrace();
        }


    }

    @Override
    public int createTables() {

        int numberofTablesInserted = 0;

        String query_create_MenuItems = "CREATE TABLE IF NOT EXISTS MenuItems( " +
                "itemID INT NOT NULL, " +
                "itemName VARCHAR(40), " +
                "cuisine VARCHAR(20), " +
                "price INT, " +
                "PRIMARY KEY (itemID));";

        String query_create_Ingredients = "CREATE TABLE IF NOT EXISTS Ingredients( " +
                "ingredientID INT NOT NULL, " +
                "ingredientName VARCHAR(40), " +
                "PRIMARY KEY (ingredientID));";

        String query_create_Includes = "CREATE TABLE IF NOT EXISTS Includes( " +
                "itemID INT NOT NULL, " +
                "ingredientID INT NOT NULL, " +
                "FOREIGN KEY (itemID) REFERENCES MenuItems(itemID) " +
                "ON DELETE CASCADE, " +
                "FOREIGN KEY (ingredientID) REFERENCES Ingredients(ingredientID) " +
                "ON DELETE CASCADE, " +
                "PRIMARY KEY (itemID, ingredientID));";

        String query_create_Ratings = "CREATE TABLE IF NOT EXISTS Ratings(" +
                "ratingID INT NOT NULL, " +
                "itemID INT, " +
                "rating INT, " +
                "ratingDate DATE, " +
                "FOREIGN KEY (itemID) REFERENCES MenuItems(itemID) " +
                "ON DELETE SET NULL, " +
                "PRIMARY KEY (ratingID));";

        String query_create_DietaryCategories = "CREATE TABLE IF NOT EXISTS DietaryCategories( " +
                "ingredientID INT NOT NULL, " +
                "dietaryCategory VARCHAR(20) NOT NULL, " +
                "FOREIGN KEY (ingredientID) REFERENCES Ingredients(ingredientID) " +
                "ON DELETE CASCADE, " +
                "PRIMARY KEY (ingredientID, dietaryCategory)); ";


        try{
            Statement statement = connection.createStatement();
            statement.executeUpdate(query_create_MenuItems);
            numberofTablesInserted++;
            statement.close();
        }catch (SQLException e) {
            e.printStackTrace();
        }

        try{
            Statement statement = connection.createStatement();
            statement.executeUpdate(query_create_Ingredients);
            numberofTablesInserted++;
            statement.close();
        }catch (SQLException e) {
            e.printStackTrace();
        }
        try{
            Statement statement = connection.createStatement();
            statement.executeUpdate(query_create_Includes);
            numberofTablesInserted++;
            statement.close();
        }catch (SQLException e) {
            e.printStackTrace();
        }
        try{
            Statement statement = connection.createStatement();
            statement.executeUpdate(query_create_Ratings);
            numberofTablesInserted++;
            statement.close();
        }catch (SQLException e) {
            e.printStackTrace();
        }
        try{
            Statement statement = connection.createStatement();
            statement.executeUpdate(query_create_DietaryCategories);
            numberofTablesInserted++;
            statement.close();
        }catch (SQLException e) {
            e.printStackTrace();
        }

        return numberofTablesInserted;
    }

    @Override
    public int dropTables() {

        int numberofTablesDropped = 0;

        String DropMenuItems = "drop table if exists MenuItems;";

        String DropIngredients = "drop table if exists Ingredients;";

        String DropIncludes = "drop table if exists Includes;";

        String DropRatings = "drop table if exists Ratings;";

        String DropDietaryCategories = "drop table if exists DietaryCategories;";


        try {
            Statement statement = connection.createStatement();

            statement.executeUpdate(DropRatings);
            numberofTablesDropped++;

            statement.executeUpdate(DropDietaryCategories);
            numberofTablesDropped++;

            statement.executeUpdate(DropIncludes);
            numberofTablesDropped++;

            statement.executeUpdate(DropMenuItems);
            numberofTablesDropped++;

            statement.executeUpdate(DropIngredients);
            numberofTablesDropped++;

            //close
            statement.close();

        } catch (SQLException e) {
            e.printStackTrace();
        }
        return numberofTablesDropped;
    }

    @Override
    public int insertMenuItems(MenuItem[] items) {

        int RowsInserted = 0;

        for (int i = 0; i < items.length; i++)
        {
            try {
                MenuItem item = items[i];

                PreparedStatement prep=connection.prepareStatement("insert into MenuItems values(?,?,?,?)");
                prep.setInt(1, item.getItemID());
                prep.setString(2,item.getItemName());
                prep.setString(3,item.getCuisine());
                prep.setInt(4,item.getPrice());

                prep.executeUpdate();

                //Close
                prep.close();
                RowsInserted++;

            }
            catch (SQLException e) {
                e.printStackTrace();
            }
        }

        return RowsInserted;
    }

    @Override
    public int insertIngredients(Ingredient[] ingredients) {

        int RowsInserted = 0;

        for (int i = 0; i < ingredients.length; i++)
        {
            try {
                Ingredient item = ingredients[i];

                PreparedStatement prep=connection.prepareStatement("insert into Ingredients values(?,?)");
                prep.setInt(1, item.getIngredientID());
                prep.setString(2,item.getIngredientName());

                prep.executeUpdate();

                //Close
                prep.close();
                RowsInserted++;

            }
            catch (SQLException e) {
                e.printStackTrace();
            }
        }

        return RowsInserted;
    }

    @Override
    public int insertIncludes(Includes[] includes) {

        int RowsInserted = 0;

        for (int i = 0; i < includes.length; i++)
        {
            try {
                Includes item = includes[i];

                PreparedStatement prep=connection.prepareStatement("insert into Includes values(?,?)");
                prep.setInt(1, item.getItemID());
                prep.setInt(2, item.getIngredientID());

                prep.executeUpdate();

                //Close
                prep.close();
                RowsInserted++;

            }
            catch (SQLException e) {
                e.printStackTrace();
            }
        }

        return RowsInserted;
    }

    @Override
    public int insertDietaryCategories(DietaryCategory[] categories) {

        int RowsInserted = 0;

        for (int i = 0; i < categories.length; i++)
        {
            try {
                DietaryCategory item = categories[i];

                PreparedStatement prep= connection.prepareStatement("insert into DietaryCategories values(?,?)");
                prep.setInt(1, item.getIngredientID());
                prep.setString(2, item.getDietaryCategory());

                prep.executeUpdate();

                //Close
                prep.close();
                RowsInserted++;

            }
            catch (SQLException e) {
                e.printStackTrace();
            }
        }

        return RowsInserted;
    }

    @Override
    public int insertRatings(Rating[] ratings) {

        int RowsInserted = 0;

        for (int i = 0; i < ratings.length; i++)
        {
            try {
                Rating item = ratings[i];

                PreparedStatement prep= connection.prepareStatement("insert into Ratings values(?,?,?,?)");
                prep.setInt(1, item.getRatingID());
                prep.setInt(2,item.getItemID());
                prep.setInt(3,item.getRating());
                prep.setString(4,item.getRatingDate());

                prep.executeUpdate();

                //Close
                prep.close();
                RowsInserted++;

            }
            catch (SQLException e) {
                e.printStackTrace();
            }
        }

        return RowsInserted;
    }

    /**
     * Should return menu items that include the given ingredient
     *
     * @return MenuItem[]
     */
    public MenuItem[] getMenuItemsWithGivenIngredient(String name){
        {
            ResultSet res;

            ArrayList<MenuItem> tmp_array  = new ArrayList<>();

            String get_query = "SELECT DISTINCT * " +
                                "FROM MenuItems M " +
                                "WHERE M.itemID IN (SELECT DISTINCT Inc.itemID " +
                                                    "FROM Ingredients Ing, Includes Inc " +
                                                    "WHERE Ing.ingredientName = \"" + name + "\" AND Ing.ingredientID = Inc.ingredientID) " +
                    "ORDER BY M.itemID asc;";

            try {
                Statement st = connection.createStatement();
                res = st.executeQuery(get_query);

                while(res.next()){
                    int itemID = res.getInt("itemID");
                    String itemName = res.getString("itemName");
                    String cuisine = res.getString("cuisine");
                    int price = res.getInt("price");
                    MenuItem a = new MenuItem(itemID, itemName, cuisine, price);
                    tmp_array.add(a);
                }

                //Close
                st.close();

            } catch (SQLException e) {
                e.printStackTrace();
            }

            MenuItem[] resarray = new MenuItem[tmp_array.size()];
            for(int i = 0; i < tmp_array.size(); i++){
                resarray[i] = tmp_array.get(i);
            }
            return resarray;
        }
    }

    /**
     * Should return menu items that do not have any ingredients in the Includes table
     *
     * @return MenuItem[]
     */
    public MenuItem[] getMenuItemsWithoutAnyIngredient(){
        {
            ResultSet res;
            ArrayList<MenuItem> tmp_array  = new ArrayList<>();

            String get_query = "SELECT DISTINCT * " +
                                "FROM MenuItems M " +
                                "WHERE M.itemID NOT IN " +
                                                    "(SELECT I.itemID " +
                                                    "From Includes I )" +
                    "ORDER BY M.itemID;";

            try {
                Statement st = connection.createStatement();
                res = st.executeQuery(get_query);

                while(res.next()){
                    int itemID = res.getInt("itemID");
                    String itemName = res.getString("itemName");
                    String cuisine = res.getString("cuisine");
                    int price = res.getInt("price");
                    MenuItem a = new MenuItem(itemID, itemName, cuisine, price);
                    tmp_array.add(a);
                }

                //Close
                st.close();

            } catch (SQLException e) {
                e.printStackTrace();
            }

            MenuItem[] resarray = new MenuItem[tmp_array.size()];
            for(int i = 0; i < tmp_array.size(); i++){
                resarray[i] = tmp_array.get(i);
            }
            return resarray;
        }
    }


    /**
     * Should return ingredients that are not included in any menu item
     *
     * @return Ingredient[]
     */
    public Ingredient[] getNotIncludedIngredients(){
        {
            ResultSet res;
            ArrayList<Ingredient> tmp_array  = new ArrayList<>();

            String get_query = "SELECT * " +
                    "FROM Ingredients Ing " +
                    "WHERE Ing.ingredientID NOT IN " +
                                            "(SELECT Ing2.ingredientID " +
                                            "FROM Ingredients Ing2, Includes Ic " +
                                            "WHERE Ing2.ingredientID = Ic.ingredientID) " +
                    "ORDER BY Ing.ingredientID;";

            try {
                Statement st = connection.createStatement();
                res = st.executeQuery(get_query);

                while(res.next()){
                    int ingredientID = res.getInt("ingredientID");
                    String ingredientName = res.getString("ingredientName");
                    Ingredient a = new Ingredient(ingredientID, ingredientName);
                    tmp_array.add(a);
                }

                //Close
                st.close();

            } catch (SQLException e) {
                e.printStackTrace();
            }

            Ingredient[] resarray = new Ingredient[tmp_array.size()];
            for(int i = 0; i < tmp_array.size(); i++){
                resarray[i] = tmp_array.get(i);
            }
            return resarray;
        }
    }


    /**
     * Should return menu item with the highest different ingredient number
     *
     * @return MenuItem
     */
    public MenuItem getMenuItemWithMostIngredients(){
        {
            ResultSet res;
            ArrayList<MenuItem> tmp_array  = new ArrayList<>();

            String get_query = "SELECT * " +
                    "FROM MenuItems M " +
                    "WHERE M.itemID = (SELECT CC.itemID " +
                                        "FROM (SELECT I.itemID , count(*) as A FROM Includes I group by I.itemID) as CC " +
                                        "WHERE CC.A = (SELECT MAX(CC.A) " +
                                                        "FROM (SELECT I.itemID , count(*) as A FROM Includes I group by I.itemID) as CC ));";

            try {
                Statement st = connection.createStatement();
                res = st.executeQuery(get_query);

                while(res.next()){
                    int itemID = res.getInt("itemID");
                    String itemName = res.getString("itemName");
                    String cuisine = res.getString("cuisine");
                    int price = res.getInt("price");
                    MenuItem a = new MenuItem(itemID, itemName, cuisine, price);
                    tmp_array.add(a);
                }

                //Close
                st.close();

            } catch (SQLException e) {
                e.printStackTrace();
            }

            MenuItem[] resarray = new MenuItem[tmp_array.size()];
            for(int i = 0; i < tmp_array.size(); i++){
                resarray[i] = tmp_array.get(i);
            }
            return resarray[0];
        }
    }



    /**
     * Should return menu items with their ID, name, and average rating
     *
     * @return MenuItemAverageRatingResult[]
     */
    public QueryResult.MenuItemAverageRatingResult[] getMenuItemsWithAvgRatings(){
        {
            ResultSet res;
            ArrayList<QueryResult.MenuItemAverageRatingResult> tmp_array  = new ArrayList<>();

            String get_query = "SELECT M.itemID, M.itemName, AVG(R.rating) as avgRating " +
                    "FROM MenuItems M LEFT JOIN Ratings R ON M.itemID = R.itemID " +
                    "GROUP BY M.itemID " +
                    "ORDER BY avgRating DESC;";

            try {
                Statement st = connection.createStatement();
                res = st.executeQuery(get_query);

                while(res.next()){
                    String itemID = res.getString("itemID");
                    String itemName = res.getString("itemName");
                    String avgRating = res.getString("avgRating");
                    QueryResult.MenuItemAverageRatingResult a = new QueryResult.MenuItemAverageRatingResult(itemID, itemName, avgRating);
                    tmp_array.add(a);
                }

                //Close
                st.close();

            } catch (SQLException e) {
                e.printStackTrace();
            }

            QueryResult.MenuItemAverageRatingResult[] resarray = new QueryResult.MenuItemAverageRatingResult[tmp_array.size()];
            for(int i = 0; i < tmp_array.size(); i++){
                resarray[i] = tmp_array.get(i);
            }
            return resarray;
        }
    }


    /**
     * Should return menu items that are suitable for a given dietary category
     *
     * @return MenuItem[]
     */
    public MenuItem[] getMenuItemsForDietaryCategory(String category){
        {
            ResultSet res;
            ArrayList<MenuItem> tmp_array  = new ArrayList<>();

            String get_query = "SELECT DISTINCT M2.* " +
                                "FROM MenuItems M2, Includes I " +
                                "WHERE M2.itemID = I.itemID AND I.itemID NOT IN " +
                                                    "(SELECT DISTINCT I2.itemID " +
                                                    "FROM Includes I2, DietaryCategories D " +
                                                    "WHERE I2.ingredientID = D.ingredientID AND D.ingredientID IN (SELECT D2.ingredientID " +
                                                                                                                 "FROM DietaryCategories D2 " +
                                                                                                                 "WHERE D2.ingredientID NOT IN (SELECT D3.ingredientID " +
                                                                                                                                                "FROM DietaryCategories D3 " +
                                                                                                                                                "WHERE D3.dietaryCategory = \"" + category + "\") ) )" +
                                "ORDER BY M2.itemID ASC ;";

            try {
                Statement st = connection.createStatement();
                res = st.executeQuery(get_query);

                while(res.next()){
                    int itemID = res.getInt("itemID");
                    String itemName = res.getString("itemName");
                    String cuisine = res.getString("cuisine");
                    int price = res.getInt("price");
                    MenuItem a = new MenuItem(itemID, itemName, cuisine, price);
                    tmp_array.add(a);
                }

                //Close
                st.close();

            } catch (SQLException e) {
                e.printStackTrace();
            }

            MenuItem[] resarray = new MenuItem[tmp_array.size()];
            for(int i = 0; i < tmp_array.size(); i++){
                resarray[i] = tmp_array.get(i);
            }
            return resarray;
        }
    }

    /**
     * Should return the most used ingredient across all menu items
     *
     * @return Ingredient
     */
    public Ingredient getMostUsedIngredient(){
        {
            ResultSet res;
            ArrayList<Ingredient> tmp_array  = new ArrayList<>();

            String get_query = "SELECT Ing.ingredientID, Ing.ingredientName " +
                    "FROM (SELECT I.ingredientID, count(*) AS c " +
                            "FROM Includes I " +
                            "GROUP BY I.ingredientID) as cc, Ingredients Ing " +
                    "WHERE Ing.ingredientID = cc.ingredientID and cc.c = (SELECT MAX(cc.c) " +
                                                                          "FROM (SELECT I.ingredientID, count(*) AS c " +
                                                                                "FROM Includes I " +
                                                                                "GROUP BY I.ingredientID) as cc);";

            try {
                Statement st = connection.createStatement();
                res = st.executeQuery(get_query);

                while(res.next()){
                    int ingredientID = res.getInt("ingredientID");
                    String ingredientName = res.getString("ingredientName");
                    Ingredient a = new Ingredient(ingredientID, ingredientName);
                    tmp_array.add(a);
                }

                //Close
                st.close();

            } catch (SQLException e) {
                e.printStackTrace();
            }

            Ingredient[] resarray = new Ingredient[tmp_array.size()];
            for(int i = 0; i < tmp_array.size(); i++){
                resarray[i] = tmp_array.get(i);
            }
            return resarray[0];
        }
    }


    /**
     * Should return cuisine names with their average ratings
     *
     * @return CuisineWithAverageResult[]
     */
    public QueryResult.CuisineWithAverageResult[] getCuisinesWithAvgRating(){
        {
            ResultSet res;
            ArrayList<QueryResult.CuisineWithAverageResult> tmp_array  = new ArrayList<>();

            String get_query = "SELECT M.cuisine, AVG(R.rating) AS averageRating " +
                    "FROM MenuItems M LEFT JOIN Ratings R ON M.itemID = R.itemID " +
                    "GROUP BY M.cuisine " +
                    "ORDER BY averageRating DESC ;";

            try {
                Statement st = connection.createStatement();
                res = st.executeQuery(get_query);

                while(res.next()){
                    String cuisine = res.getString("cuisine");
                    String averageRating = res.getString("averageRating");
                    QueryResult.CuisineWithAverageResult a = new QueryResult.CuisineWithAverageResult(cuisine, averageRating);
                    tmp_array.add(a);
                }

                //Close
                st.close();

            } catch (SQLException e) {
                e.printStackTrace();
            }

            QueryResult.CuisineWithAverageResult[] resarray = new QueryResult.CuisineWithAverageResult[tmp_array.size()];
            for(int i = 0; i < tmp_array.size(); i++){
                resarray[i] = tmp_array.get(i);
            }
            return resarray;
        }
    }


    /**
     * Should return cuisine names with their average ingredient count per item
     *
     * @return CuisineWithAverageResult[]
     */
    public QueryResult.CuisineWithAverageResult[] getCuisinesWithAvgIngredientCount(){
        {
            ResultSet res;
            ArrayList<QueryResult.CuisineWithAverageResult> tmp_array  = new ArrayList<>();

            String get_query = "SELECT I_items.cuisine, (I_items.C / M_items.C) AS averageCount " +
                    "FROM (SELECT M.cuisine AS cuisine, COUNT(*) AS C " +
                            "FROM MenuItems M " +
                            "GROUP BY M.cuisine) AS M_items , " +

                            "(SELECT M.cuisine AS cuisine, COUNT(*) AS C " +
                            "FROM MenuItems M ,Includes I " +
                            "WHERE M.itemID = I.itemID " +
                            "GROUP BY M.cuisine " +
                            "UNION " +
                            "SELECT M2.cuisine AS cuisine , 0 AS C " +
                            "FROM MenuItems M2 " +
                            "WHERE M2.cuisine NOT IN (SELECT M3.cuisine " +
                                                    "FROM MenuItems M3, Includes I3 " +
                                                    "WHERE M3.itemID = I3.itemID)) AS I_items " +
                    "WHERE I_items.cuisine = M_items.cuisine " +
                    "ORDER BY averageCount desc;";

            try {
                Statement st = connection.createStatement();
                res = st.executeQuery(get_query);

                while(res.next()){
                    String cuisine = res.getString("cuisine");
                    String averageCount = res.getString("averageCount");
                    QueryResult.CuisineWithAverageResult a = new QueryResult.CuisineWithAverageResult(cuisine, averageCount);
                    tmp_array.add(a);
                }

                //Close
                st.close();

            } catch (SQLException e) {
                e.printStackTrace();
            }

            QueryResult.CuisineWithAverageResult[] resarray = new QueryResult.CuisineWithAverageResult[tmp_array.size()];
            for(int i = 0; i < tmp_array.size(); i++){
                resarray[i] = tmp_array.get(i);
            }
            return resarray;
        }
    }


    /**
     * Should increase the price of all menu items that include the given ingredient by the given amount
     *
     * @return the row count for SQL Data Manipulation Language (DML) statements
     */
    public int increasePrice(String ingredientName, String increaseAmount){

        String update_query = "UPDATE MenuItems M " +
                "SET M.price = M.price + \"" + increaseAmount + "\" " +
                "WHERE M.itemID IN (SELECT Inc.itemID " +
                                    "FROM  Ingredients Ing , Includes Inc " +
                                    "WHERE Ing.ingredientID = Inc.ingredientID AND Ing.ingredientName = \"" + ingredientName + "\" );";

        int number_of_updates = 0;

        try{
            Statement statement = connection.createStatement();
            number_of_updates = statement.executeUpdate(update_query);

            statement.close();
        }catch (SQLException e) {
            e.printStackTrace();
        }

        return number_of_updates;

    }

    /**
     * Should delete and return ratings that have an earlier rating date than the given date
     *
     * @return Rating[]
     */
    public Rating[] deleteOlderRatings(String date){

        ResultSet res;
        int res1;
        ArrayList<Rating> tmp_array  = new ArrayList<>();

        String out_query = "SELECT * " +
                "FROM Ratings R " +
                "WHERE R.ratingDate < \"" + date + "\" " +
                "ORDER BY R.ratingID asc;";

        String delete_query = "DELETE FROM Ratings R " +
                "WHERE R.ratingDate < \"" + date + "\" ;";

        try{
            Statement st = connection.createStatement();
            res = st.executeQuery(out_query);
            while(res.next()){
                int ratingID = res.getInt("ratingID");
                int itemID = res.getInt("itemID");
                int rating = res.getInt("rating");
                String ratingDate = res.getString("ratingDate");
                Rating a = new Rating(ratingID, itemID, rating, ratingDate);
                tmp_array.add(a);
            }
            st.close();
        }catch (SQLException e) {
            e.printStackTrace();
        }

        try{
            Statement statement = connection.createStatement();
            res1 = statement.executeUpdate(delete_query);

            statement.close();
        }catch (SQLException e) {
            e.printStackTrace();
        }

        Rating[] resarray = new Rating[tmp_array.size()];
        for(int i = 0; i < tmp_array.size(); i++){
            resarray[i] = tmp_array.get(i);
        }
        return resarray;
    }
}