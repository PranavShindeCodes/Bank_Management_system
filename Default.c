#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_USERS 100

typedef struct {
    int accountNumber;   // Unique Account Number
    char aadhaar[20];
    char pan[20];
    char phone[15];
    char address[100];
    char password[20];
    char accountType[10]; // "Saving" or "Current"
    double balance;
} User;

char nominee[30], dob[30], branch[30];

// Function prototypes
void registerUser();
int loginUser();
void depositMoney(int userIndex);
void withdrawMoney(int userIndex);
void showTransactions(int userIndex);
void transferMoney(int userIndex);
void saveTransaction(int accountNumber, double amount, char* type);
void loadUsers();
void saveUsers();

// Global users array
User users[MAX_USERS];
int userCount = 0;
int nextAccountNumber = 1001;  // First Account Number starts from 1001

int main() {
    int choice, loggedInUser = -1;
    loadUsers();

    while (1) {
        if (loggedInUser == -1) {
            printf("Bank Management System \n1. Register\n2. Login\n3. Exit\nChoose: ");
            scanf("%d", &choice);
            if (choice == 1) {
                registerUser();
            } else if (choice == 2) {
                loggedInUser = loginUser();
            } else {
                saveUsers();
                exit(0);
            }
        } else {
            printf("\n1. Deposit Money\n2. Withdraw Money\n3. View Transactions\n4. Transfer Money\n5. Logout\nChoose: ");
            scanf("%d", &choice);
            if (choice == 1) {
                depositMoney(loggedInUser);
            } else if (choice == 2) {
                withdrawMoney(loggedInUser);
            } else if (choice == 3) {
                showTransactions(loggedInUser);
            } else if (choice == 4) {
                transferMoney(loggedInUser);
            } else {
                loggedInUser = -1;
            }
        }
    }
    return 0;
}

// Register new user
void registerUser() {
    if (userCount >= MAX_USERS) {
        printf("User limit reached!\n");
        return;
    }

    User newUser;
    newUser.accountNumber = nextAccountNumber++;

    printf("Enter branch name\n");
    scanf("%s", branch);
    printf("Enter Aadhaar: ");
    scanf("%s", newUser.aadhaar);
    printf("Enter PAN: ");
    scanf("%s", newUser.pan);
    printf("Enter Phone: ");
    scanf("%s", newUser.phone);
    printf("enter nominee\n");
    scanf("%s", nominee);
    printf("enter dob\n");
    scanf("%s", dob);
    getchar();  // To consume leftover newline
    printf("Enter Address: ");
    fgets(newUser.address, sizeof(newUser.address), stdin);
    newUser.address[strcspn(newUser.address, "\n")] = 0;

    printf("Set Password: ");
    scanf("%s", newUser.password);

    int accType;
    printf("Choose Account Type (1. Saving, 2. Current): ");
    scanf("%d", &accType);

    if (accType == 1) {
        strcpy(newUser.accountType, "Saving");
        newUser.balance = 2000.0;
    } else {
        strcpy(newUser.accountType, "Current");
        newUser.balance = 10000.0;
    }

    users[userCount++] = newUser;
    saveTransaction(newUser.accountNumber, newUser.balance, "Account Opening");
    saveUsers();

    printf("Account Created Successfully! Your Account Number: %d\n", newUser.accountNumber);
}

// Login function
int loginUser() {
    int accountNumber;
    char password[20];
    printf("Enter Account Number: ");
    scanf("%d", &accountNumber);
    printf("Enter Password: ");
    scanf("%s", password);

    for (int i = 0; i < userCount; i++) {
        if (users[i].accountNumber == accountNumber && strcmp(users[i].password, password) == 0) {
            printf("Login Successful!\n");
            return i;
        }
    }
    printf("Invalid Credentials!\n");
    return -1;
}

// Deposit money
void depositMoney(int userIndex) {
    double amount;
    printf("Enter Amount to Deposit: ");
    scanf("%lf", &amount);

    users[userIndex].balance += amount;
    saveTransaction(users[userIndex].accountNumber, amount, "Deposit");
    saveUsers();

    printf("₹%.2lf Deposited Successfully!\n", amount);
}

// Withdraw money
void withdrawMoney(int userIndex) {
    double amount;
    printf("Enter Amount to Withdraw: ");
    scanf("%lf", &amount);

    if (users[userIndex].balance >= amount) {
        users[userIndex].balance -= amount;
        saveTransaction(users[userIndex].accountNumber, amount, "Withdraw");
        saveUsers();
        printf("₹%.2lf Withdrawn Successfully!\n", amount);
    } else {
        printf("Insufficient Balance!\n");
    }
}

// View transactions + show balance
void showTransactions(int userIndex) {
    char filename[30];
    sprintf(filename, "%d.txt", users[userIndex].accountNumber);
    FILE *file = fopen(filename, "r");

    if (file == NULL) {
        printf("No transactions found!\n");
        return;
    }

    char line[100];
    printf("Transaction History:\n");
    while (fgets(line, sizeof(line), file)) {
        printf("%s", line);
    }
    fclose(file);

    // Show current balance
    printf("\nCurrent Balance: ₹%.2lf\n", users[userIndex].balance);
}

// Transfer money between accounts
void transferMoney(int userIndex) {
    int receiverAcc;
    double amount;
    printf("Enter Receiver's Account Number: ");
    scanf("%d", &receiverAcc);
    printf("Enter Amount to Transfer: ");
    scanf("%lf", &amount);

    if (users[userIndex].balance < amount) {
        printf("Insufficient Balance!\n");
        return;
    }

    int receiverIndex = -1;
    for (int i = 0; i < userCount; i++) {
        if (users[i].accountNumber == receiverAcc) {
            receiverIndex = i;
            break;
        }
    }

    if (receiverIndex == -1) {
        printf("Receiver not found!\n");
        return;
    }

    users[userIndex].balance -= amount;
    users[receiverIndex].balance += amount;

    saveTransaction(users[userIndex].accountNumber, amount, "Sent Money");
    saveTransaction(users[receiverIndex].accountNumber, amount, "Received Money");
    saveUsers();

    printf("₹%.2lf Transferred Successfully!\n", amount);
}

// Save transactions to file
void saveTransaction(int accountNumber, double amount, char* type) {
    char filename[30];
    sprintf(filename, "%d.txt", accountNumber);
    FILE *file = fopen(filename, "a");

    if (file == NULL) return;

    time_t now = time(NULL);
    fprintf(file, "%s - ₹%.2lf (%s)\n", ctime(&now), amount, type);
    fclose(file);
}

// Load users from file
void loadUsers() {
    FILE *file = fopen("users.dat", "rb");
    if (file != NULL) {
        fread(&userCount, sizeof(int), 1, file);
        fread(&nextAccountNumber, sizeof(int), 1, file);
        fread(users, sizeof(User), userCount, file);
        fclose(file);
    }
}

// Save users to file
void saveUsers() {
    FILE *file = fopen("users.dat", "wb");
    fwrite(&userCount, sizeof(int), 1, file);
    fwrite(&nextAccountNumber, sizeof(int), 1, file);
    fwrite(users, sizeof(User), userCount, file);
    fclose(file);
}
