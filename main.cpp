#define CROW_MAIN

#include "crow_all.h"
#include <iostream>
#include <regex>
using namespace std;
using namespace crow;

#define NOT_FOUND response(404, "File not found.")

response loadFile(string filepath){

    //open file
    ifstream file(filepath);

    //error handling
    if(!file){
        return NOT_FOUND;
    }

    //read the file into a buffer
    ostringstream buffer;
    buffer << file.rdbuf();

    //return new response
    return response(200, buffer.str());
}

response addToCart(string product, int quantity){

    //open file
    ofstream file("/var/MyServer/cart.txt", ios::app);

    //error handling
    if(!file){
        return NOT_FOUND;
    }

    regex spaceRegex("%20");
    product = regex_replace(product, spaceRegex, " ");

    //make string to add to cart
    stringstream cartEntry;
    cartEntry << product << " x" << quantity << endl;

    //append to file
    file << cartEntry.str();

    //close file
    file.close();

    //return new response
    return response(200, to_string(quantity) + "x " + product + " added to cart.");

}

void authenticate(string username, string password, response& res){

    if (username != "denim"){
        res.code = 401;
        res.write("Invalid username");
    }
    else if (password != "1234"){
        res.code = 401;
        res.write("Invalid password");
    }
    else{
        res.code = 202;
        res.write("Logged in successfully");
    }

    res.end();
}

int main()
{
    cout << "starting server\n";

    crow::SimpleApp app;

    //main page
    CROW_ROUTE(app, "/")
    ([](){
        return loadFile("../public/index.html");
    });

    //fetch images
    CROW_ROUTE(app, "/images/<string>")
    ([](string filename){
        return loadFile("../public/images/" + filename);
    });

    //fetch css
    CROW_ROUTE(app, "/styles/<string>")
    ([](string filename){
        return loadFile("../public/styles/" + filename);
    });

    //fetch scripts
    CROW_ROUTE(app, "/scripts/<string>")
    ([](string filename){
        return loadFile("../public/scripts/" + filename);
    });

    //fetch products
    CROW_ROUTE(app, "/products/<string>")
    ([](string filename){
        return loadFile("../public/" + filename + ".html");
    });

    //add to cart
    CROW_ROUTE(app, "/cart/<string>/<int>")
    ([](string product, int quantity){


        //bad quantity
        if(quantity < 1){
            return response("Quantity must be greater than 0");
        }

    
        return addToCart(product, quantity);
    });

    //get cart
    CROW_ROUTE(app, "/cart").methods(HTTPMethod::Get)
    ([](){

        //load the cart text file from the docker server
        return loadFile("/var/MyServer/cart.txt");
    });

    //checkout cart
    CROW_ROUTE(app, "/checkout")
    ([](){

        //load the cart text file from the docker server
        auto r = loadFile("../public/checkout.html");
        r.code = 402;
        return r;
    });

    //authenticate
    CROW_ROUTE(app, "/authenticate").methods(HTTPMethod::Post)
    ([](request& req, response& res){

        string username = req.get_body_params().get("username");
        string password = req.get_body_params().get("password");

        //load the cart text file from the docker server
        authenticate(username, password, res);
    });

    //checkout cart
    CROW_ROUTE(app, "/complete_checkout")
    ([](){
        //load the cart text file from the docker server
        auto r = loadFile("/var/MyServer/cart.txt");
        r.code = 402;
        return r;
    });

    app.port(23500).multithreaded().run();
    return 0;
}

