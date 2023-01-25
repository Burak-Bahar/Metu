#include <iostream>
#include <string>
#include "highSpeedTrain.h"
 using namespace std;
/**
 * Empty Constructor
 * Initializes attributes according to the default route, namely "Florence -> Rome in 90 mins.".
 **/
HighSpeedTrain::HighSpeedTrain(){
    // YOUR CODE HERE
    from = "Florence";
    to = "Rome";
    eta = 90;
    
}

/*
 * Constructor
 * Fills the attributes according to given values.
 *
 * @param from std::string starting location of the travel
 * @param to std::string destination of the travel
 * @param eta int estimated time of arrival in minutes.
 */
HighSpeedTrain::HighSpeedTrain(std::string from, std::string to, int eta){
    // YOUR CODE HERE
    this->from = from;
    this->to = to;
    this->eta = eta;
    
}

/*
 * Copy Constructor
 * Fills the attributes according to attributes of the given object with one exception.
 * Since it requires an extra time to replace a train, the eta of the train will be delayed 60 mins.
 *
 * @param h HighSpeedTrain train that is going to be replaced in the route.
 * @return this new train with updated eta.
 */
HighSpeedTrain::HighSpeedTrain(const HighSpeedTrain &h){
    // YOUR CODE HERE
    from = h.from;
    to = h.to;
    eta = h.eta +60;
    
}

/*
 * Stream Extraction Overload
 * Formats the output of a given train as "source -> destination in # mins."
 *
 * @param output std::ostream output stream
 * @param h HighSpeedTrain the train whose route is going to be summarized.
 */
std::ostream &operator<<(std::ostream &output, const HighSpeedTrain &h){
    // YOUR CODE HERE
    cout << h.from << " -> " << h.to << " in " << h.eta << " mins.";
    return output;
}

/*
 * Addition Overload
 * This operator will be used to represent transfers.
 * If the destination of second train does not match with the source of the first one,
 * then it produces the string of "Transfer is not possible!" with NO NEW LINE AT THE END.
 * Otherwise it combines the routes and calculate the total eta and produces the string in
 * the form of "source1 -> destination1 -> destination2 in # mins."
 *
 * @param h1 HighSpeedTrain the first train
 * @param h2 HighSpeedTrain the second train
 * @return one of the strings as explained above.
 */
std::string operator+(const HighSpeedTrain &h1, const HighSpeedTrain &h2){
    // YOUR CODE HERE
    string s;
    if(h1.to == h2.from){
        int t = h1.eta + h2.eta;
        s += h1.from;
        s += " -> ";
        s += h2.from;
        s += " -> ";
        s += h2.to;
        s += " in ";
        s += to_string(t);
        s += " mins.";
        return s;
    }
    else{
        s = "Transfer is not possible!";
        return s;
    }
     // ADJUST THIS AFTER YOUR IMPLEMENTATION IS DONE.
}

/*
 * Comparison Overloads
 * This operator will be used to order the summaries of train.
 * The priority for comparison is here:
 * 1. Compare the source of the trains alphabetically (ex. Florence < Milano)
 * 2. Compare the destination of the trains alphabetically (the same example above)
 * 3. Compare the eta's of the trains, the train with less eta will be smaller.
 *
 * @param h1, h2 HighSpeedTrain the trains to be compared.
 */
bool operator<(const HighSpeedTrain &h1, const HighSpeedTrain &h2){
    // YOUR CODE HERE
    if( h1.from < h2.from){
        return true;
    }
    else if(h1.from == h2.from){
        if(h1.to < h2.to){
            return true;
        }
        else if(h1.to == h2.to){
            if(h1.eta < h2.eta){
                return true;
            }
        }
    }
    return false; // ADJUST THIS AFTER YOUR IMPLEMENTATION IS DONE.
}

bool operator>(const HighSpeedTrain &h1, const HighSpeedTrain &h2){
    // YOUR CODE HERE
    if( h1.from > h2.from){
        return true;
    }
    else if(h1.from == h2.from){
        if(h1.to > h2.to){
            return true;
        }
        else if(h1.to == h2.to){
            if(h1.eta > h2.eta){
                return true;
            }
        }
    }
    return false;  // ADJUST THIS AFTER YOUR IMPLEMENTATION IS DONE.
}
