// Program:     CheckDB
// Author:      Eric Stevenson
// Version:     08/02/2022
// Description: Checks counts of the user-selected database against the Pre-Production database to ensure refresh was successful.

#include <iostream>
#include <algorithm>
#include <vector>
#include <string>
#include <fstream>
#include <cstring>
#include <string>
#include <fstream>
#include <stdio.h>

using namespace std;

void fileCompare(string userDB, string sourceDB); //Check if output files match
void compareLines(string userDB, string sourceDB); //Compare line-by-line to provide specific tables with differences 

int main()
{
    int select; //menu selection
    string userDB;
    string sourceDB;
    string userid;
    string password;
    string connection;

    //remove old input files
    remove("C:\\astra2\\intgtest\\checkDB\\preProd.txt");
    remove("C:\\astra2\\intgtest\\checkDB\\userDB.txt");

    //list of ACM/ATP databases. Input should be limited to this selection.
    std::vector<std::string> acmDB{
        "RVA2CM","RVBFCM","RVCDCM","RVCLCM","RVCTCM","RVGDCM","RVI2CM","RVI3CM","RVI4CM","RVI5CM",
        "RVI6CM","RVI7CM","RVI8CM","RVI9CM","RVIFCM","RVIGCM","RVIHCM","RVIJCM","RVQ50M","RVQ51M",
        "RVQ52M","RVPRCM","RVRPCM","RVTRCM","RVPPCM"
    };
    std::vector<std::string> atpDB{
        "RVA2T2","RVBFT2","RVCLT2","RVCTT2","RVF1T2","RVGDCS","RVGDT2","RVGDTT","RVI2T2","RVI3T2",
        "RVI4T2","RVI5T2","RVI6T2","RVI7T2","RVI8T2","RVI9T2","RVI9T2","RVIAT2","RVIBT2","RVIFT2",
        "RVIGT2","RVIHT2","RVIJT2","RVR1T2","RVR2T2","RVR3T2","RVRPT2","RVR4T2","RVS1T2","RVS2T2",
        "RVS3T2","RVSTAG","RVW2T2","RVWPT2","RVQ01P","RVQ02P","RVQ03P","RVQ04P","RVQ50P","RVQ51P",
        "RVQ52P","RVPRT2","RVP2T2"
    };
    //menu
    cout << endl;
    cout << ". . . . . . . . . . . . . . . . ." << endl;
    cout << ". Select an environment:        ." << endl;
    cout << ".                               ." << endl;
    cout << ". 1)  ACM                       ." << endl;
    cout << ". 2)  ATP                       ." << endl;
    cout << ". 3)  Exit                      ." << endl;
    cout << ". . . . . . . . . . . . . . . . ." << endl << endl;
 
    cout << "Make a selection: ";
    cin.clear();
    cin >> select;

    if (cin.fail()) {
        cin.clear();
        cin.ignore();
    }
    //Choose ACM or ATP
    switch (select) {
        //ACM
        case 1:     do {
                        cout << "Enter your ACM database:" << endl;
                        std::cin >> userDB;
                        std::transform(userDB.begin(), userDB.end(), userDB.begin(), ::toupper); //cast input to UPPER
                        if (!(std::find(std::begin(acmDB), std::end(acmDB), userDB) != std::end(acmDB))) {
                            cout << "'" + userDB + "' " + "is not a valid ACM Test database." << endl;
                        }
                    } while (!(std::find(std::begin(acmDB), std::end(acmDB), userDB) != std::end(acmDB))); //limit input to valid databases
                    cout << "Enter your username for the " + userDB + " database:" << endl;
                    cin >> userid;
                    cout << "Enter your password for the " + userid + " account:" << endl;
                    cin >> password;
                    //connect to user-specified database
                    connection = "sqlplus -S -L " + userid + "/" + password + "@" + userDB + " @C:\\astra2\\intgtest\\checkDB\\ACMrefresh.sql";
                    system((connection).c_str());
                    system("cls");
                    //connect to Pre-Production database
                    sourceDB = "RVPPCM";
                    system("sqlplus -S -L astracm/gotit@rvppcm @C:\\astra2\\intgtest\\checkDB\\SourceACM.sql");
                    system("cls");
                    fileCompare(userDB, sourceDB); //check for differences between databases
                    break;
        //ATP
        case 2:     do {
                        cout << "Enter your ATP database:" << endl;
                        std::cin >> userDB;
                        std::transform(userDB.begin(), userDB.end(), userDB.begin(), ::toupper); //cast input to UPPER
                        if (!(std::find(std::begin(atpDB), std::end(atpDB), userDB) != std::end(atpDB))) {
                            cout << "'" + userDB + "' " + "is not a valid ATP Test database." << endl;
                        }
                    } while (!(std::find(std::begin(atpDB), std::end(atpDB), userDB) != std::end(atpDB))); //limit input to valid databases
                    cout << "Enter your username for the " + userDB + " database:" << endl;
                    cin >> userid;
                    cout << "Enter your password for the " + userid + " account:" << endl;
                    cin >> password;
                    //connect to user-specified database
                    connection = "sqlplus -S -L " + userid + "/" + password + "@" + userDB + " @C:\\astra2\\intgtest\\checkDB\\ATPrefresh.sql";
                    system((connection).c_str());
                    system("cls");
                    //connect to Pre-Production database
                    sourceDB = "RVP2T2";
                    system("sqlplus -S -L srvstart/srvstart@rvp2t2 @C:\\astra2\\intgtest\\checkDB\\SourceATP.sql");
                    system("cls");
                    fileCompare(userDB, sourceDB); //check for differences between databases
                    break;
        case 3:     exit(0); //exit application
                    break;
        default:    cout << "Wrong input." << endl;
    }
    system("PAUSE");
    system("cls");
    main();
}

//compare output files to check if there are any count differences
void fileCompare(string userDB, string sourceDB) {
    fstream f1, f2;
    char c1, c2;
    string result;
    char response;
    string file1 = "C:\\astra2\\intgtest\\checkDB\\userDB.txt";
    string file2 = "C:\\astra2\\intgtest\\checkDB\\preProd.txt";
    int flag = 3;
    //open files
    f1.open(file1, ios::in);
    f2.open(file2, ios::in);

    if (f1.fail()){
        cout << "There was a problem connecting to " << userDB << ". Please try again." << endl;
        f2.close();
        system("PAUSE");
        system("cls");
        main();
    };

    if (f2.fail()) {
        cout << "There was a problem connecting to " << sourceDB << ". Please try again." << endl;
        f1.close();
        system("PAUSE");
        system("cls");
        main();
    };

    //compare files
    while (1) {
        c1 = f1.get();
        c2 = f2.get();

        if (c1 != c2) {
            flag = 0;
            break;
        }
        if ((c1 == EOF) || (c2 == EOF)) {
            break;
        }
    }
    //close files
    f1.close();
    f2.close();
    //tell user if there are differences
    if (flag){cout << "The counts in " + userDB + " match those in " + sourceDB + "." << endl;}
    else{
        cout << "The counts in " + userDB + " DO NOT match those in " + sourceDB + "." << endl;
        //give user option to view differences
        cout << "Would you like to view the results? ";
        cin >> result;
        response = result[0];
        //"YES" if response starts with a "y", "NO" if it starts with anything else
        if (response == 'Y' or response == 'y'){
            compareLines(userDB, sourceDB); 
        }  
    }
    //delete output files
    remove("C:\\astra2\\intgtest\\checkDB\\preProd.txt");
    remove("C:\\astra2\\intgtest\\checkDB\\userDB.txt");

    return;
}

//compare each line and provide feedback on specific tables with differences
void compareLines(string userDB, string sourceDB) {
    ifstream file1, file2;
    char string1[256], string2[256];
    //open files
    file1.open("C:\\astra2\\intgtest\\checkDB\\preProd.txt", ios::binary);
    file2.open("C:\\astra2\\intgtest\\checkDB\\userDB.txt", ios::binary);
    //list differences
    cout << endl << "The following tables have different counts in " << sourceDB << " and " << userDB << ":" << endl;
    //compare and print lines with differences until end of 1st file is reached
    while (!file1.eof()) {
        file1.getline(string1, 256);
        file2.getline(string2, 256);

        if (strcmp(string1, string2) != 0) {
            cout << sourceDB << ": " << string1 << "\n";
            cout << userDB << ": " << string2 << "\n";
            cout << endl;
        }
    }
    //close files
    file1.close();
    file2.close();

    return;
}
