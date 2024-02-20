// Thomas Martin - CIS4301 - Homework2 #5

#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>

using namespace std;
struct ToyCarOrder {

    int orderNumber = 0;
    int quantityOrdered = 0;
    double priceEach = 0.0;
    int orderLineNumber = 0;
    double sales = 0.0;
    string date;
    int daysSinceLastOrder = 0;
    string productLine;
    string customerName;
    string addressLine1;
    string city;
    string postalCode;
    string country;
    string contactFirstName;
    string contactLastName;
    string dealSize;
};

string& EscapeSingleQuote(string& str) {
    size_t pos = 0;
    while ((pos = str.find('\'', pos)) != std::string::npos) {
        str.replace(pos, 1, "''");
        pos += 2;
    }
    return str;
}

vector<string> SQLizeInserts(vector<ToyCarOrder>& orders) {
    vector<string> inserts;

    for (auto& order : orders) {
        string sql = "INSERT INTO ToyCarOrdersAndSales VALUES (";
        sql += to_string(order.orderNumber) + ", ";
        sql += to_string(order.quantityOrdered) + ", ";
        sql += to_string(order.priceEach) + ", ";
        sql += to_string(order.orderLineNumber) + ", ";
        sql += to_string(order.sales) + ", ";
        sql += "TO_DATE('" + order.date + "', 'dd/mm/yyyy'), ";
        sql += to_string(order.daysSinceLastOrder) + ", '";
        sql += EscapeSingleQuote(order.productLine) + "', '";
        sql += EscapeSingleQuote(order.customerName) + "', '";
        sql += EscapeSingleQuote(order.addressLine1) + "', '";
        sql += order.city + "', '";
        sql += order.postalCode + "', '";
        sql += order.country + "', '";
        sql += EscapeSingleQuote(order.contactLastName) + "', '";
        sql += EscapeSingleQuote(order.contactFirstName) + "', '";
        sql += order.dealSize;
        sql += "');\n";
        inserts.push_back(sql);
    }
    return inserts;
}

void WriteInsertsToFile(const vector<string>& inserts, const string& path) {

    // output to file
    ofstream output(path);
    if (output.is_open()) {

        // set define off so we don't get substitution issues on ampersand
        output << "SET DEFINE OFF;\n";

        for (const auto& insert : inserts) {
            output << insert;
        }
        cout << "Output written to file, insert_commands.txt" << endl;
    }
    else {
        cout << "Error: Unable to open file!" << endl;
    }
}

vector<ToyCarOrder> ReadOrdersFromFile(const string& path) {

    // vector to store orders
    vector<ToyCarOrder> orders;

    // read file
    ifstream input(path);

    // check if the file opened
    if (input.is_open()) {

        // define a string to be the next line
        string nextLine;

        // read line of input file to burn the headers
        getline(input, nextLine);

        unsigned int lineCounter = 1;

        // loop until we reach the end of the file
        while (getline(input, nextLine)) {

            // create a a string stream object to break apart the line at each comma
            istringstream stream(nextLine);

            try {
                // new toycarorder object to read data into and push into vector
                ToyCarOrder newOrder;

                // read the line from stream up to comma and store result in token string
                string token;

                getline(stream, token, ',');
                newOrder.orderNumber = stoi(token);

                getline(stream, token, ',');
                newOrder.quantityOrdered = stoi(token);

                getline(stream, token, ',');
                newOrder.priceEach = stod(token);

                getline(stream, token, ',');
                newOrder.orderLineNumber = stoi(token);

                getline(stream, token, ',');
                newOrder.sales = stod(token);

                getline(stream, newOrder.date, ',');

                getline(stream, token, ',');
                newOrder.daysSinceLastOrder = stoi(token);

                // unused status
                getline(stream, token, ',');

                getline(stream, newOrder.productLine, ',');

                // unused msrp
                getline(stream, token, ',');

                // unused product code
                getline(stream, token, ',');

                // customer name
                getline(stream, token, ',');

                string temp = token;

                // opening quote found, but no closing , comma inside quote, don't count as delimiter
                if (count(token.begin(), token.end(), '"') == 1) {

                    // store the name so far plus the comma we removed with getline
                    temp += ',';

                    // get the line up to the next "
                    getline(stream, token, '"');

                    // add the rest of the name
                    temp += token;

                    // consume comma
                    getline(stream, token, ',');

                    if (!temp.empty()) {
                        temp = temp.substr(1);
                    }
                }

                newOrder.customerName = temp;

                // unused phone
                getline(stream, token, ',');

                // address could contain quotes and multiple commas
                getline(stream, token, ',');

                temp = token;

                // opening quote found, but no closing , comma inside quote, don't count as delimiter
                if (count(token.begin(), token.end(), '"') == 1) {

                    // store the address so far plus the comma we removed with getline
                    temp += ',';

                    // get the line up to the next "
                    getline(stream, token, '"');

                    // add the rest of the name
                    temp += token;

                    // consume comma
                    getline(stream, token, ',');

                    if (!temp.empty()) {
                        temp = temp.substr(1);
                    }
                }
                newOrder.addressLine1 = temp;

                getline(stream, newOrder.city, ',');
                getline(stream, newOrder.postalCode, ',');
                getline(stream, newOrder.country, ',');
                getline(stream, newOrder.contactFirstName, ',');
                getline(stream, newOrder.contactLastName, ',');
                getline(stream, newOrder.dealSize);
                newOrder.dealSize.erase(newOrder.dealSize.find_last_not_of(" \t\n\r\f\v") + 1);

                // add order to vector
                orders.push_back(newOrder);
            }
            catch (...) {
                cout << "Error: Reading data on line " << lineCounter << endl;
            }

            ++lineCounter;
        }
    }
    else {
        cout << "Error: File not opened" << endl;
    }

    return orders;
}

int main()
{

    // load orders
    auto orders = ReadOrdersFromFile("./Auto_Sales_data.csv");

    cout << "Number of Orders Found: " << orders.size() << endl;

    if (orders.size() > 0) {

        // write to file
        WriteInsertsToFile(SQLizeInserts((orders)), "insertCommands.txt");
    }

    return 0;
}