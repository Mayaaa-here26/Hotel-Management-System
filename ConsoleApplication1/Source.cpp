#include "stdafx.h"
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <limits>
#include <cstdlib>
#include <ctime>

using namespace std;

// MACRO + INLINE
#define TAX_RATE 0.05
inline double addTax(double x) 
{ return x * (1 + TAX_RATE); }

// ENUM
enum RoomStatus { AVAILABLE = 0, OCCUPIED = 1 };
enum RoomType { SINGLE = 1, DOUBLE = 2, VIP_SUITE = 3, APARTMENT = 4 };

// STRUCT
struct Booking {
    int id;
    string name;
    int roomNumber;
    RoomStatus status;
    RoomType type;
    double amount;
};

// GLOBALS
const int FLOORS = 2;
const int ROOMS = 5;
double getRoomPrice(RoomType t) {
    switch (t) {
    case SINGLE: return 80.00;
    case DOUBLE: return 100.50;
    case VIP_SUITE: return 180.00;
    case APARTMENT: return 250.00;
    }
    return 100.00;
}

int roomMap[FLOORS][ROOMS];
vector<Booking> bookings;

// INPUT VALIDATION
int readInt(const string& msg) {
    while (true) {
        cout << msg;
        int x;
        if (cin >> x) {
            // consume the remaining newline so future getline works predictably
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return x;
        }
        cout << "Invalid integer. Try again.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

double readDouble(const string& msg) {
    while (true) {
        cout << msg;
        double x;
        if (cin >> x) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return x;
        }
        cout << "Invalid number. Try again.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

bool adminLogin() {
    vector<pair<string, string>> admins;

    // ---------- LOAD ADMINS ----------
    ifstream file("admin.txt");
    if (file.is_open()) {
        string line;
        while (getline(file, line)) {
            size_t pos = line.find(',');
            if (pos != string::npos) {
                string u = line.substr(0, pos);
                string p = line.substr(pos + 1);
                if (!u.empty() && u.back() == '\r') u.pop_back();
                if (!p.empty() && p.back() == '\r') p.pop_back();
                admins.push_back(pair<string, string>(u, p)); // <-- use constructor
            }
        }
        file.close();
    }

    // ---------- LOGIN LOOP ----------
    while (true) {
        string inputUser, inputPass;
        cout << "Enter admin username: ";
        getline(cin, inputUser);
        cout << "Enter admin password: ";
        getline(cin, inputPass);

        bool found = false;
        for (size_t i = 0; i < admins.size(); i++) {
            if (admins[i].first == inputUser && admins[i].second == inputPass) {
                cout << "Admin login successful.\n";
                return true;
            }
        }

        cout << "Invalid credentials.\n";
        char choice;
        cout << "Do you want to create a new admin account? (y/n): ";
        cin >> choice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        if (choice == 'y' || choice == 'Y') {
            string newUser, newPass;
            cout << "New admin username: ";
            getline(cin, newUser);
            cout << "New admin password: ";
            getline(cin, newPass);

            // Append to file
            ofstream out("admin.txt", ios::app);
            out << newUser << "," << newPass << "\n";
            out.close();

            admins.push_back(pair<string, string>(newUser, newPass)); // <-- use constructor
            cout << "New admin account created.\n";
        } else {
            cout << "Try login again.\n";
        }
    }

    return false; // technically unreachable
}


/* ---------------- ROOMS ---------------- */
void initRooms() {
    for (int f = 0; f < FLOORS; f++)
        for (int r = 0; r < ROOMS; r++)
            roomMap[f][r] = AVAILABLE;
}

void showRooms() {
    cout << "\n------- ROOM STATUS --------\n";
    for (int f = 0; f < FLOORS; f++) {
        cout << "Floor " << f + 1 << ": ";
        for (int r = 0; r < ROOMS; r++)
            cout << roomMap[f][r] << " ";
        cout << endl;
    }

    cout << "\n------- BOOKINGS --------\n";
    if (bookings.empty()) {
        cout << "No bookings yet.\n";
    }
    else {
        for (auto& b : bookings) {

            // Convert RoomType to text
            string typeName;
            switch (b.type) {
                case SINGLE: typeName = "Single"; break;
                case DOUBLE: typeName = "Double"; break;
                case VIP_SUITE: typeName = "VIP Suite"; break;
                case APARTMENT: typeName = "Apartment"; break;
                default: typeName = "Unknown"; break;
            }

            cout << "ID: " << b.id
                << " | Name: " << b.name
                << " | Type: " << typeName
                << " | Room: " << b.roomNumber
                << " | Amount: " << fixed << setprecision(2) << b.amount
                << endl;
        }
    }
}
/* ---------------- BOOKING ---------------- */
int generateID() { return rand() % 90000 + 10000; }

void makeBooking() {
    string name;
    cout << "\nEnter customer name: ";
    getline(cin, name);

    showRooms();

    int floor = readInt("Select floor (1-2): ");
    if (floor < 1 || floor > FLOORS) { cout << "Invalid floor.\n"; return; }

    int room = readInt("Select room number (1-5): ");
    if (room < 1 || room > ROOMS) { cout << "Invalid room.\n"; return; }

    int f = floor - 1, r = room - 1;
    if (roomMap[f][r] == OCCUPIED) {
        cout << "Room already occupied.\n";
        return;
    }

    cout << "\nChoose Room Type:\n";
    cout << "1. Single\n2. Double\n3. VIP Suite\n4. Apartment\n";

    int typeChoice = readInt("Select room type: ");
    if (typeChoice < 1 || typeChoice > 4) {
        cout << "Invalid type.\n";
        return;
    }

    RoomType type = static_cast<RoomType>(typeChoice);
    double roomPrice = getRoomPrice(type);

    cout << "\nRoom price per night: " << fixed << setprecision(2) << roomPrice << endl;

    int nights = readInt("How many nights? ");
    double amount = roomPrice * nights;

    amount += rand() % 50;      // random fee
    amount = addTax(amount);    // apply tax

    Booking b;
    b.id = generateID();
    b.name = name;
    b.roomNumber = (f * ROOMS) + r + 1;
    b.status = OCCUPIED;
    b.type = type;
    b.amount = amount;

    bookings.push_back(b);
    roomMap[f][r] = OCCUPIED;

    cout << "\nBooking Successful!\n";
    cout << "ID: " << b.id 
         << " | Room: " << b.roomNumber
         << " | Total: " << fixed << setprecision(2) << amount << endl;
}
/* ---------------- CHECKOUT ---------------- */
void checkout() {
    int id;
    cout << "\nEnter booking ID to remove: ";
    if (!(cin >> id)) {
        cout << "Invalid input.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return;
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    for (int i = 0; i < (int)bookings.size(); i++) {
        if (bookings[i].id != id) continue;

        int rn = bookings[i].roomNumber - 1;
        int f = rn / ROOMS;
        int r = rn % ROOMS;
        if (f >= 0 && f < FLOORS && r >= 0 && r < ROOMS)
            roomMap[f][r] = AVAILABLE;

        cout << "Checked out: " << bookings[i].name << endl;
        bookings.erase(bookings.begin() + i);
        cout << "Booking removed.\n";
        return;
    }
    cout << "Booking ID not found.\n";
}

/* ---------------- SAVE/LOAD ---------------- */
void saveData() {
    ofstream file("bookings.txt");
    // Save simple CSV: id,name,roomNumber,amount,type
    for (auto& b : bookings)
        file << b.id << "," << b.name << "," << b.roomNumber << "," << fixed << setprecision(2) << b.amount << "," << (int)b.type << "\n";
    cout << "Bookings saved.\n";
}

void loadData() {
    ifstream file("bookings.txt");
    if (!file.is_open()) { cout << "No previous data found.\n"; return; }

    bookings.clear();
    initRooms();

    string line;
    int lineNo = 0;
    while (getline(file, line)) {
        lineNo++;
        if (line.empty()) continue;

        try {
            stringstream ss(line);
            Booking b;
            string temp;

            // id
            if (!getline(ss, temp, ',')) throw runtime_error("Missing id");
            b.id = stoi(temp);

            // name
            if (!getline(ss, b.name, ',')) throw runtime_error("Missing name");

            // roomNumber
            if (!getline(ss, temp, ',')) throw runtime_error("Missing roomNumber");
            b.roomNumber = stoi(temp);

            // amount
            if (!getline(ss, temp, ',')) throw runtime_error("Missing amount");
            b.amount = stod(temp);

            // type (optional) - if missing, default to SINGLE
            int t = SINGLE;
            if (getline(ss, temp, ',')) {
                try { t = stoi(temp); }
                catch (...) { t = SINGLE; }
            }
            if (t < SINGLE || t > APARTMENT) t = SINGLE;
            b.type = static_cast<RoomType>(t);

            b.status = OCCUPIED;

            // mark room occupied (guard against bad indices)
            int rn = b.roomNumber - 1;
            int f = rn / ROOMS;
            int r = rn % ROOMS;
            if (f >= 0 && f < FLOORS && r >= 0 && r < ROOMS)
                roomMap[f][r] = OCCUPIED;

            bookings.push_back(b);
        }
        catch (const exception& e) {
            cout << "Warning: skipped invalid bookings line " << lineNo << ". (" << e.what() << ")\n";
            continue;
        }
    }
    cout << "Data loaded.\n";
}

/* ---------------- MENUS ---------------- */
void menuAdmin() {
    cout << "\n==== ADMIN MENU ====\n";
    cout << "1. Show Rooms\n2. Make Booking\n3. Checkout\n4. Save Data\n5. Load Data\n6. Exit\n";
}

void menuGuest() {
    cout << "\n==== GUEST MENU ====\n";
    cout << "1. Show Rooms\n2. Make Booking\n3. Exit\n";
}

/* ---------------- MAIN ---------------- */
int main() {
    srand((unsigned)time(0));
    initRooms();
    loadData();

cout << "=====HOTEL MANAGMENT SYSTEM====="<<endl;
  cout << "Login as:\n1. Admin\n2. Guest\nChoose: ";
    int userType;
    if (!(cin >> userType)) {
        cout << "Invalid input. Exiting.\n";
        return 0;
    }
    // consume leftover newline so subsequent getline calls behave correctly
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    if (userType == 1) {

    bool loggedIn = false;
    while (!loggedIn) {
        loggedIn = adminLogin();
        if (!loggedIn) {
            cout << "Invalid credentials. Try again.\n";
        }
    }

    while (true) {
        menuAdmin();
        int choice = readInt("Choose: ");
        switch (choice) {
        case 1: showRooms(); break;
        case 2: makeBooking(); break;
        case 3: checkout(); break;
        case 4: saveData(); break;
        case 5: loadData(); break;
        case 6:
            cout << "Goodbye!\n";
            saveData();
            return 0;
        default:
            cout << "Invalid option.\n";
        }
    }
}

    else if (userType == 2) {
        cout << "Guest login successful.\n";
        while (true) {
            menuGuest();
            int choice = readInt("Choose: ");
            switch (choice) {
            case 1: showRooms(); break;
            case 2: makeBooking(); break;
            case 3: cout << "Goodbye!\n"; return 0;
            default: cout << "Invalid option.\n";
            }
        }
    }
    else {
        cout << "Invalid user type.\n";
    }

	system("pause");
	return 0;
}