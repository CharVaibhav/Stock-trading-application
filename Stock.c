#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <conio.h>
#include <windows.h>
#else
#include <unistd.h>
#include <termios.h>
#endif

#define MAX_USERS 100
#define MAX_TRANSACTIONS 100
#define MAX_STOCKS 10
#define MAX_WATCHLIST 10


typedef struct {
    char username[50];
    char password[50];
    float balance;
} User;


typedef struct {
    char symbol[10];
    float price;
} Stock;


typedef struct {
    char type[10];
    char stock[10];
    float amount;
    float price;
    float quantity;
} Transaction;

typedef struct {
    char stock[10];
    float totalInvested;
    float totalEarned;
    float totalProfitOrLoss;
} StockPerformance;


typedef struct {
    char symbol[10];
} Watchlist;


void registerUser(User users[], int *numUsers);
void authenticate(User users[], int numUsers, User **authenticatedUser);
void displayPortfolio(const User *user);
void buyStock(User *user, const Stock stocks[], int numStocks, Transaction transactions[], int *numTransactions);
void sellStock(User *user, const Stock stocks[], int numStocks, Transaction transactions[], int *numTransactions);
void displayMenu();
void viewStock(const Stock stocks[], int numStocks);
void viewTransactionHistory(const Transaction transactions[], int numTransactions);
void manageWatchlist(Watchlist watchlist[], int *numWatchlist);
void setColor(int color);
void viewPerformanceAnalytics(Transaction transactions[], int numTransactions, const char *stockSymbol);
void getPassword(char *password, int maxLen);

int main() {
    User users[MAX_USERS];
    int numUsers = 0, numTransactions, numWatchlist;
    User *authenticatedUser = NULL;
    Transaction transactions[MAX_TRANSACTIONS] = {
        {"Buy", "AAPL", 700, 700, 1},
        {"Buy", "AAPL", 300, 300, 1},
        {"Sell", "AAPL", 1000, 1000, 1}
    };
    numTransactions = 3;
    Watchlist watchlist[MAX_WATCHLIST];  
    numWatchlist = 0;

    
    Stock stocks[MAX_STOCKS] = {
        {"AAPL", 150.0},
        {"GOOGL", 2800.0},
        {"AMZN", 3500.0},
        {"MSFT", 300.0},
        {"TSLA", 700.0}
    };
    int numStocks = 5;

    int choice;
    do {
        printf("\n1. Register\n2. Login\n3. Exit\nEnter your choice: ");
        scanf("%d", &choice);
        switch (choice) {
            case 1:
                registerUser(users, &numUsers);
                break;
            case 2:
                authenticate(users, numUsers, &authenticatedUser);
                if (authenticatedUser != NULL) {
                    int userChoice;
                    do {
                        displayMenu();
                        scanf("%d", &userChoice);
                        switch (userChoice) {
                            case 1:
                                displayPortfolio(authenticatedUser);
                                break;
                            case 2:
                                viewStock(stocks, numStocks);
                                break;
                            case 3:
                                buyStock(authenticatedUser, stocks, numStocks, transactions, &numTransactions);
                                break;
                            case 4:
                                sellStock(authenticatedUser, stocks, numStocks, transactions, &numTransactions);
                                break;
                            case 5:
                                viewTransactionHistory(transactions, numTransactions);
                                break;
                            case 6:
                                manageWatchlist(watchlist, &numWatchlist);
                                break;
                            case 7:
                                {
                                    char stockSymbol[10];
                                    printf("Enter stock symbol to view performance analytics: ");
                                    scanf("%s", stockSymbol);
                                    viewPerformanceAnalytics(transactions, numTransactions, stockSymbol);
                                    break;
                                }
                            case 8:
                                printf("Logging out...\n");
                                break;
                            default:
                                printf("Invalid choice. Please try again.\n");
                        }
                    } while (userChoice != 8);
                }
                break;
            case 3:
                printf("Exiting...\n");
                break;
            default:
                printf("Invalid choice. Please try again.\n");
        }
    } while (choice != 3);

    return 0;
}


void registerUser(User users[], int *numUsers) {
    if (*numUsers >= MAX_USERS) {
        printf("User limit reached. Cannot register more users.\n");
        return;
    }

    char username[50];
    char password[50];

    printf("Enter new username: ");
    scanf("%s", username);
    printf("Enter new password: ");
    getPassword(password, 50);

    strcpy(users[*numUsers].username, username);
    strcpy(users[*numUsers].password, password);
    users[*numUsers].balance = 1000.0; 
    (*numUsers)++;

    printf("\nUser registered successfully!\n");
}

void authenticate(User users[], int numUsers, User **authenticatedUser) {
    char username[50];
    char password[50];
    int i;

    printf("Username: ");
    scanf("%s", username);
    printf("Password: ");
    getPassword(password, 50);

    for (i = 0; i < numUsers; i++) {
        if (strcmp(users[i].username, username) == 0 && strcmp(users[i].password, password) == 0) {
            printf("\nAuthentication successful!\n");
            *authenticatedUser = &users[i];
            return;
        }
    }

    printf("\nAuthentication failed. Invalid username or password.\n");
    *authenticatedUser = NULL;
}

void displayPortfolio(const User *user) {
    setColor(14); 
    printf("\n---- Portfolio ----\n");
    setColor(7); 
    printf("Username: %s\n", user->username);
    printf("Balance: $%.2f\n", user->balance);
}

void buyStock(User *user, const Stock stocks[], int numStocks, Transaction transactions[], int *numTransactions) {
    char symbol[10];
    float amount;
    int i;
    printf("Enter the stock symbol to buy: ");
    scanf("%s", symbol);

    int stockIndex = -1;
    for (i = 0; i < numStocks; i++) {
        if (strcmp(stocks[i].symbol, symbol) == 0) {
            stockIndex = i;
            break;
        }
    }

    if (stockIndex == -1) {
        printf("Invalid stock symbol.\n");
        return;
    }

    printf("Enter the amount to invest: ");
    scanf("%f", &amount);

    if (amount > user->balance) {
        printf("Insufficient balance!\n");
        return;
    }

    user->balance -= amount;
    float quantity = amount / stocks[stockIndex].price;

    if (*numTransactions < MAX_TRANSACTIONS) {
        strcpy(transactions[*numTransactions].type, "Buy");
        strcpy(transactions[*numTransactions].stock, stocks[stockIndex].symbol);
        transactions[*numTransactions].amount = amount;
        transactions[*numTransactions].price = stocks[stockIndex].price;
        transactions[*numTransactions].quantity = quantity;
        (*numTransactions)++;
    }

    printf("You bought %.2f worth of %s shares.\n", amount, stocks[stockIndex].symbol);
}

void sellStock(User *user, const Stock stocks[], int numStocks, Transaction transactions[], int *numTransactions) {
    char symbol[10];
    float amount;
    int i;
    printf("Enter the stock symbol to sell: ");
    scanf("%s", symbol);

    int stockIndex = -1;
    for (i = 0; i < numStocks; i++) {
        if (strcmp(stocks[i].symbol, symbol) == 0) {
            stockIndex = i;
            break;
        }
    }

    if (stockIndex == -1) {
        printf("Invalid stock symbol.\n");
        return;
    }

    printf("Enter the amount to sell: ");
    scanf("%f", &amount);

    user->balance += amount;
    float quantity = amount / stocks[stockIndex].price;

    if (*numTransactions < MAX_TRANSACTIONS) {
        strcpy(transactions[*numTransactions].type, "Sell");
        strcpy(transactions[*numTransactions].stock, stocks[stockIndex].symbol);
        transactions[*numTransactions].amount = amount;
        transactions[*numTransactions].price = stocks[stockIndex].price;
        transactions[*numTransactions].quantity = quantity;
        (*numTransactions)++;
    }

    printf("You sold %.2f worth of %s shares.\n", amount, stocks[stockIndex].symbol);
}

void displayMenu() {
    setColor(11); 
    printf("\n---- Menu ----\n");
    setColor(7); 
    printf("1. View Portfolio\n");
    printf("2. View Stock\n");
    printf("3. Buy Stock\n");
    printf("4. Sell Stock\n");
    printf("5. View Transaction History\n");
    printf("6. Manage Watchlist\n");
    printf("7. View Performance Analytics\n");
    printf("8. Logout\n");
    printf("Enter your choice: ");
}

void viewStock(const Stock stocks[], int numStocks) {
	int i;
    setColor(10); 
    printf("\n---- Available Stocks ----\n");
    setColor(7); 
    printf("Symbol\tPrice\n");
    for (i = 0; i < numStocks; i++) {
        printf("%s\t$%.2f\n", stocks[i].symbol, stocks[i].price);
    }
}

void viewTransactionHistory(const Transaction transactions[], int numTransactions) {
	int i;
    setColor(13); 
    printf("\n---- Transaction History ----\n");
    setColor(7); 
    printf("Type\tStock\tAmount\tPrice\tQuantity\n");
    for (i = 0; i < numTransactions; i++) {
        printf("%s\t%s\t$%.2f\t$%.2f\t%.2f\n", transactions[i].type, transactions[i].stock, transactions[i].amount, transactions[i].price, transactions[i].quantity);
    }
}

void manageWatchlist(Watchlist watchlist[], int *numWatchlist) {
	int i, j;
    setColor(12); 
    printf("\n---- Manage Watchlist ----\n");
    setColor(7); 

    int choice;
    do {
        printf("1. Add stock to watchlist\n");
        printf("2. Remove stock from watchlist\n");
        printf("3. View watchlist\n");
        printf("4. Back to main menu\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                if (*numWatchlist < MAX_WATCHLIST) {
                    printf("Enter stock symbol to add to watchlist: ");
                    scanf("%s", watchlist[*numWatchlist].symbol);
                    (*numWatchlist)++;
                } else {
                    printf("Watchlist is full. Cannot add more stocks.\n");
                }
                break;
            case 2:
                if (*numWatchlist > 0) {
                    char symbol[10];
                    printf("Enter stock symbol to remove from watchlist: ");
                    scanf("%s", symbol);

                    int i, found = 0;
                    for (i = 0; i < *numWatchlist; i++) {
                        if (strcmp(watchlist[i].symbol, symbol) == 0) {
                            found = 1;
                            for (j = i; j < *numWatchlist - 1; j++) {
                                strcpy(watchlist[j].symbol, watchlist[j + 1].symbol);
                            }
                            (*numWatchlist)--;
                            printf("Stock %s removed from watchlist.\n", symbol);
                            break;
                        }
                    }

                    if (!found) {
                        printf("Stock symbol not found in watchlist.\n");
                    }
                } else {
                    printf("Watchlist is empty. No stocks to remove.\n");
                }
                break;
            case 3:
                printf("\n---- Watchlist ----\n");
                for (i = 0; i < *numWatchlist; i++) {
                    printf("%s\n", watchlist[i].symbol);
                }
                break;
            case 4:
                printf("Returning to main menu...\n");
                break;
            default:
                printf("Invalid choice. Please try again.\n");
        }
    } while (choice != 4);
}

void viewPerformanceAnalytics(Transaction transactions[], int numTransactions, const char *stockSymbol) {
    float totalInvested = 0.0;
    float totalEarned = 0.0;
    int i;

    for (i = 0; i < numTransactions; i++) {
        if (strcmp(transactions[i].stock, stockSymbol) == 0) {
            if (strcmp(transactions[i].type, "Buy") == 0) {
                totalInvested += transactions[i].amount;
            } else if (strcmp(transactions[i].type, "Sell") == 0) {
                totalEarned += transactions[i].amount;
            }
        }
    }

    float totalProfitOrLoss = totalEarned - totalInvested;

    printf("\nStock: %s\n", stockSymbol);
    printf("Total Invested: $%.2f\n", totalInvested);
    printf("Total Earned: $%.2f\n", totalEarned);
    printf("Profit/Loss: $%.2f\n", totalProfitOrLoss);
}

void getPassword(char *password, int maxLen) {
#ifdef _WIN32
    int i = 0;
    while (1) {
        int ch = _getch();
        if (ch == '\r' || ch == '\n') {
            break;
        }
        if (ch == '\b' && i > 0) {
            printf("\b \b");
            i--;
        } else if (ch != '\b') {
            password[i++] = ch;
            printf("*");
        }
    }
    password[i] = '\0';
#else
    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    fgets(password, maxLen, stdin);
    password[strcspn(password, "\n")] = 0; 
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
#endif
}

void setColor(int color) {
#ifdef _WIN32
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
#else
    printf("\033[1;%dm", color);
#endif
}
