#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

typedef struct
{
    char name[25];
    int num;
    int balance;
    char accountType[10];
} Account;

void create_account();
void deposit_money();
void withdraw_money();
void check_balance();
void read_accounts();

int main()
{
    int OTP, userOTP, option;

    // Generate OTP
    srand(time(0));
    OTP = rand() % 9000 + 1000;
    printf("OTP is %d\n", OTP);
    printf("Enter OTP: ");
    scanf("%d", &userOTP);
    if (userOTP == OTP)
    {
        printf("\033[1;32mYou Have Successfully Logged In!\033[0m\n");

        while (1)
        {
            printf("\n1. Create Account\n");
            printf("2. Deposit Money\n");
            printf("3. Withdraw Money\n");
            printf("4. Check Balance\n");
            printf("5. View All Accounts\n");
            printf("6. Exit\n");
            printf("Enter your choice: ");
            scanf("%d", &option);
            switch (option)
            {
            case 1:
                create_account();
                break;
            case 2:
                deposit_money();
                break;
            case 3:
                withdraw_money();
                break;
            case 4:
                check_balance();
                break;
            case 5:
                read_accounts();
                break;
            case 6:
                printf("\033[1;33mExiting...\033[0m\n");
                return 0;
            default:
                printf("\033[1;31mInvalid Choice. Please Try Again.\033[0m\n");
            }
        }
    }
    else
    {
        printf("\033[1;31mOTP is Not Valid\033[0m\n");
    }

    return 0;
}

void create_account()
{
    Account acc;
    int initialDeposit = 0;

    FILE *ptr = fopen("account_info.dat", "a+");
    if (ptr == NULL)
    {
        printf("\033[1;31mFile Not Found\033[0m\n");
        return;
    }

    getchar(); // Clear the newline character left in the buffer by scanf
    printf("Enter Your Name: ");
    fgets(acc.name, sizeof(acc.name), stdin);
    int ind = strcspn(acc.name, "\n"); // Remove the newline character
    acc.name[ind] = '\0';

    printf("Enter Your Account Number: ");
    scanf("%d", &acc.num);

    getchar(); // Clear the newline character left in the buffer
    printf("Choose Account Type (saving/current/minor): ");
    fgets(acc.accountType, sizeof(acc.accountType), stdin);
    ind = strcspn(acc.accountType, "\n"); // Remove the newline character
    acc.accountType[ind] = '\0';

    if (strcmp(acc.accountType, "saving") == 0 || strcmp(acc.accountType, "current") == 0)
    {
        printf("For %s accounts, minimum initial deposit is 500.\n", acc.accountType);
        do
        {
            printf("Enter Initial Deposit: ");
            scanf("%d", &initialDeposit);
            if (initialDeposit < 500)
            {
                printf("\033[1;31mInitial deposit must be at least 500.\033[0m\n");
            }
        } while (initialDeposit < 500);
    }
    else if (strcmp(acc.accountType, "minor") == 0)
    {
        printf("Minor accounts have no minimum deposit requirement.\n");
        printf("Enter Initial Deposit: ");
        scanf("%d", &initialDeposit);
    }
    else
    {
        printf("\033[1;31mInvalid Account Type! Please Try Again.\033[0m\n");
        fclose(ptr);
        return;
    }

    acc.balance = initialDeposit;
    fwrite(&acc, sizeof(acc), 1, ptr);
    fclose(ptr);

    printf("\033[1;32mAccount Created Successfully with a Balance of %d!\033[0m\n", acc.balance);
}

void deposit_money()
{
    int accNum, amount, found = 0;
    Account acc;
    FILE *ptr = fopen("account_info.dat", "r+");
    if (ptr == NULL)
    {
        printf("\033[1;31mFile Not Found\033[0m\n");
        return;
    }

    printf("Enter Account Number to Deposit Money: ");
    scanf("%d", &accNum);

    while (fread(&acc, sizeof(acc), 1, ptr))
    {
        if (acc.num == accNum)
        {
            found = 1;
            printf("Enter Amount to Deposit: ");
            scanf("%d", &amount);
            acc.balance += amount;

            fseek(ptr, -sizeof(acc), SEEK_CUR); // Move the file pointer to the current record
            fwrite(&acc, sizeof(acc), 1, ptr); // Update the record
            printf("\033[1;32mAmount Deposited Successfully! New Balance: %d\033[0m\n", acc.balance);
            break;
        }
    }

    if (!found)
    {
        printf("\033[1;31mAccount Not Found!\033[0m\n");
    }

    fclose(ptr);
}

void withdraw_money()
{
    int accNum, amount, found = 0;
    Account acc;
    FILE *ptr = fopen("account_info.dat", "r+");
    if (ptr == NULL)
    {
        printf("\033[1;31mFile Not Found\033[0m\n");
        return;
    }

    printf("Enter Account Number to Withdraw Money: ");
    scanf("%d", &accNum);

    while (fread(&acc, sizeof(acc), 1, ptr))
    {
        if (acc.num == accNum)
        {
            found = 1;
            printf("Enter Amount to Withdraw: ");
            scanf("%d", &amount);

            if (amount > acc.balance)
            {
                printf("\033[1;31mInsufficient Balance!\033[0m\n");
            }
            else
            {
                acc.balance -= amount;

                fseek(ptr, -sizeof(acc), SEEK_CUR); // Move the file pointer to the current record
                fwrite(&acc, sizeof(acc), 1, ptr); // Update the record
                printf("\033[1;32mAmount Withdrawn Successfully! New Balance: %d\033[0m\n", acc.balance);
            }
            break;
        }
    }

    if (!found)
    {
        printf("\033[1;31mAccount Not Found!\033[0m\n");
    }

    fclose(ptr);
}

void check_balance()
{
    int accNum, found = 0;
    Account acc;
    FILE *ptr = fopen("account_info.dat", "r");
    if (ptr == NULL)
    {
        printf("\033[1;31mFile Not Found\033[0m\n");
        return;
    }

    printf("Enter Account Number to Check Balance: ");
    scanf("%d", &accNum);

    while (fread(&acc, sizeof(acc), 1, ptr))
    {
        if (acc.num == accNum)
        {
            found = 1;
            printf("\033[1;34mAccount Holder: %s\nAccount Number: %d\nAccount Type: %s\nBalance: %d\033[0m\n", acc.name, acc.num, acc.accountType, acc.balance);
            break;
        }
    }

    if (!found)
    {
        printf("\033[1;31mAccount Not Found!\033[0m\n");
    }

    fclose(ptr);
}

void read_accounts()
{
    Account acc;
    FILE *ptr = fopen("account_info.dat", "r");
    if (ptr == NULL)
    {
        printf("\033[1;31mFile Not Found\033[0m\n");
        return;
    }

    printf("\n\033[1;34mList of Accounts:\033[0m\n");
    printf("%-25s %-10s %-10s %-10s\n", "Name", "Account No.", "Type", "Balance");
    printf("-------------------------------------------------------------\n");

    while (fread(&acc, sizeof(acc), 1, ptr))
    {
        printf("%-25s %-10d %-10s %-10d\n", acc.name, acc.num, acc.accountType, acc.balance);
    }

    fclose(ptr);
}
