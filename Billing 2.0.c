#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <direct.h>

// ---------------- Structures ----------------

struct Item {
    char code[10];
    char name[50];
    float price;
};

struct OrderItem {
    char code[10];
    char name[50];
    float price;
    int qty;
};

struct Order {
    char *customer;
    char date[30];
    int numOfItems;
    struct OrderItem *items;
};

struct SummaryEntry {
    char name[50];
    float total;
};

// ---------------- Global Variables ----------------

struct Item *itemDB = NULL;  // dynamic array of items
int numItems = 0;            // current count of items

// ---------------- Function Prototypes ----------------

void initializeItems();
struct Item* findItemByCode(const char *code);
void createInvoice();
void manageItems();
void generateSummary();
void saveInvoiceSummary(const char *customer, float total);
void clearInputBuffer();
void pressEnterToContinue();
void listItemCodes();

// ---------------- Utility Functions ----------------

// Flush leftover newline characters from input buffer.
void clearInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// Wait for the user to press Enter.
void pressEnterToContinue() {
    printf("\nPress Enter to continue...");
    clearInputBuffer();
    getchar();
}

// ---------------- Initialize Default Items ----------------

void initializeItems() {
    // Start with 10 Indian cuisine items.
    numItems = 10;
    itemDB = (struct Item *)malloc(20 * sizeof(struct Item)); // initial capacity for 20 items

    struct Item defaultItems[10] = {
        {"BRY01", "Biryani", 180.00},
        {"DSA02", "Dosa", 60.00},
        {"PNE03", "Paneer Butter Masala", 200.00},
        {"CHT04", "Chai", 20.00},
        {"SAM05", "Samosa", 15.00},
        {"IDY06", "Idly", 40.00},
        {"VDA07", "Vada Pav", 30.00},
        {"THL08", "Thali", 250.00},
        {"PRA09", "Paratha", 70.00},
        {"GLB10", "Gulab Jamun", 50.00}
    };

    for (int i = 0; i < 10; i++) {
        itemDB[i] = defaultItems[i];
    }
}

// ---------------- Find Item by Code ----------------

struct Item* findItemByCode(const char *code) {
    for (int i = 0; i < numItems; i++) {
        if (strcmp(itemDB[i].code, code) == 0) {
            return &itemDB[i];
        }
    }
    return NULL;
}

// ---------------- List Items (Codes & Names Only) ----------------

void listItemCodes() {
    printf("\nAvailable Items (Code and Name):\n");
    printf("--------------------------------\n");
    for (int i = 0; i < numItems; i++) {
         printf("%-8s %-30s\n", itemDB[i].code, itemDB[i].name);
    }
    printf("--------------------------------\n");
}

// ---------------- Invoice Printing Functions ----------------

void generateBillHeader(char *customer, char date[30]) {
    printf("\n===============================================\n");
    printf("\t   ADV. RESTAURANT INVOICE\n");
    printf("===============================================\n");
    printf("Customer: %s\t Date: %s\n", customer, date);
    printf("-----------------------------------------------\n");
    printf("Code   Item                      Qty    Price    Total\n");
    printf("-----------------------------------------------\n");
}

void generateBillBody(FILE *fp, char code[10], char name[50], int qty, float price) {
    printf("%-6s %-25s %-6d INR%-6.2f INR%-6.2f\n", code, name, qty, price, qty * price);
    if(fp != NULL)
        fprintf(fp, "%-6s %-25s %-6d INR%-6.2f INR%-6.2f\n", code, name, qty, price, qty * price);
}

void generateBillFooter(FILE *fp, float total) {
    float discount = 0.10f * total;
    float netTotal = total - discount;
    float tax = 0.09f * netTotal;
    float grandTotal = netTotal + (2 * tax);

    printf("-----------------------------------------------\n");
    printf("Subtotal:                  INR%.2f\n", total);
    printf("Discount (10%%):            -INR%.2f\n", discount);
    printf("Net Total:                 INR%.2f\n", netTotal);
    printf("CGST (9%%):                 INR%.2f\n", tax);
    printf("SGST (9%%):                 INR%.2f\n", tax);
    printf("-----------------------------------------------\n");
    printf("GRAND TOTAL:               INR%.2f\n", grandTotal);
    printf("===============================================\n");

    if(fp != NULL) {
        fprintf(fp, "-----------------------------------------------\n");
        fprintf(fp, "Subtotal:                  INR%.2f\n", total);
        fprintf(fp, "Discount (10%%):            -INR%.2f\n", discount);
        fprintf(fp, "Net Total:                 INR%.2f\n", netTotal);
        fprintf(fp, "CGST (9%%):                 INR%.2f\n", tax);
        fprintf(fp, "SGST (9%%):                 INR%.2f\n", tax);
        fprintf(fp, "-----------------------------------------------\n");
        fprintf(fp, "GRAND TOTAL:               INR%.2f\n", grandTotal);
        fprintf(fp, "===============================================\n");
    }
}

// ---------------- Invoice Saving Functions ----------------

void saveInvoice(struct Order *order, float total) {
    // Create Invoices directory if it doesn't exist
    _mkdir("Invoices"); 
    char filename[100];
    // Use current timestamp to generate unique filename
    sprintf(filename, "Invoices/%s_%ld.txt", order->customer, time(NULL));

    FILE *fp = fopen(filename, "w");
    if (!fp) {
        printf("Error creating invoice file.\n");
        return;
    }
    fprintf(fp, "INDIAN CUSINE DAILY\n");
    fprintf(fp, "Customer: %s\nDate: %s\n", order->customer, order->date);
    fprintf(fp, "-----------------------------------------------\n");
    fprintf(fp, "Code   Item                      Qty    Price    Total\n");
    fprintf(fp, "-----------------------------------------------\n");

    for (int i = 0; i < order->numOfItems; i++) {
        generateBillBody(fp, order->items[i].code, order->items[i].name, order->items[i].qty, order->items[i].price);
    }
    generateBillFooter(fp, total);
    fclose(fp);
    printf("Invoice saved as %s\n", filename);
}

// Append invoice summary (customer and total) to InvoiceSummary.txt
void saveInvoiceSummary(const char *customer, float total) {
    FILE *fp = fopen("InvoiceSummary.txt", "a");
    if (fp) {
        // Format: customer|total
        fprintf(fp, "%s|%.2f\n", customer, total);
        fclose(fp);
    }
}

// ---------------- Option 1: Create Invoice ----------------

void createInvoice() {
    struct Order order;
    float total = 0;
    char option;
    
    // Dynamically allocate memory for customer name
    order.customer = (char *)malloc(50 * sizeof(char));
    if (!order.customer) {
        printf("Memory allocation error!\n");
        return;
    }
    printf("Enter Customer Name: ");  
    fgets(order.customer, 50, stdin);
    order.customer[strcspn(order.customer, "\n")] = '\0';  // remove newline
    
    // Set current date (using __DATE__ macro for simplicity)
    strcpy(order.date, __DATE__);
    
    // Allocate initial memory for order items (max 50 items)
    order.items = (struct OrderItem *)malloc(50 * sizeof(struct OrderItem));
    if (!order.items) {
        printf("Memory allocation error!\n");
        free(order.customer);
        return;
    }
    order.numOfItems = 0;
    
    while (1) {
        char code[10];
        printf("\nEnter Item Code (or '0' to end, 'L' to list items): ");  
        scanf("%s", code);
        
        // Option to list items (without prices)
        if (strcmp(code, "L") == 0 || strcmp(code, "l") == 0) {
            listItemCodes();
            continue;  // Prompt again for item code
        }
        
        if (strcmp(code, "0") == 0)
            break;
        
        struct Item *item = findItemByCode(code);
        if (item != NULL) {
            strcpy(order.items[order.numOfItems].code, item->code);
            strcpy(order.items[order.numOfItems].name, item->name);
            order.items[order.numOfItems].price = item->price;
            printf("Item: %s, Price: INR%.2f\n", item->name, item->price);
        } else {
            printf("Invalid code. Enter item name manually: ");  
            clearInputBuffer();
            fgets(order.items[order.numOfItems].name, 50, stdin);
            order.items[order.numOfItems].name[strcspn(order.items[order.numOfItems].name, "\n")] = '\0';  
            printf("Enter Price: ");  
            scanf("%f", &order.items[order.numOfItems].price);
            // Save entered code as is
            strcpy(order.items[order.numOfItems].code, code);
        }
        
        printf("Enter Quantity: ");  
        scanf("%d", &order.items[order.numOfItems].qty);
        total += order.items[order.numOfItems].qty * order.items[order.numOfItems].price;
        order.numOfItems++;
    }
    
    // Display invoice on screen
    generateBillHeader(order.customer, order.date);
    for (int i = 0; i < order.numOfItems; i++) {
        generateBillBody(stdout, order.items[i].code, order.items[i].name, order.items[i].qty, order.items[i].price);
    }
    generateBillFooter(stdout, total);
    
    clearInputBuffer();
    printf("\nDo you want to save the invoice? (y/n): ");  
    scanf("%c", &option);
    if (option == 'y' || option == 'Y') {
        saveInvoice(&order, total);
        saveInvoiceSummary(order.customer, total);
    }
    
    free(order.customer);
    free(order.items);
    pressEnterToContinue();
}

// ---------------- Option 2: Manage Item Database ----------------

void listItems() {
    printf("\nCurrent Items in Database:\n");
    printf("----------------------------------------------\n");
    printf("Code     Item Name                      Price(INR)\n");
    printf("----------------------------------------------\n");
    for (int i = 0; i < numItems; i++) {
        printf("%-8s %-30s INR%.2f\n", itemDB[i].code, itemDB[i].name, itemDB[i].price);
    }
    printf("----------------------------------------------\n");
}

void addItem() {
    struct Item newItem;
    printf("Enter new item code: ");  
    scanf("%s", newItem.code);
    // Check if code already exists
    if (findItemByCode(newItem.code)) {
        printf("Item with code %s already exists.\n", newItem.code);
        return;
    }
    printf("Enter item name: ");  
    clearInputBuffer();
    fgets(newItem.name, 50, stdin);
    newItem.name[strcspn(newItem.name, "\n")] = '\0';
    printf("Enter item price (INR): ");  
    scanf("%f", &newItem.price);
    
    // Append new item to database
    itemDB[numItems] = newItem;
    numItems++;
    printf("Item added successfully!\n");
}

void updateItem() {
    char code[10];
    printf("Enter item code to update: ");  
    scanf("%s", code);
    struct Item *item = findItemByCode(code);
    if (item == NULL) {
        printf("Item with code %s not found.\n", code);
        return;
    }
    printf("Current name: %s, price: INR%.2f\n", item->name, item->price);
    printf("Enter new name: ");  
    clearInputBuffer();
    fgets(item->name, 50, stdin);
    item->name[strcspn(item->name, "\n")] = '\0';
    printf("Enter new price (INR): ");  
    scanf("%f", &item->price);
    printf("Item updated successfully!\n");
}

void deleteItem() {
    char code[10];
    printf("Enter item code to delete: ");  
    scanf("%s", code);
    int found = 0;
    for (int i = 0; i < numItems; i++) {
        if (strcmp(itemDB[i].code, code) == 0) {
            found = 1;
            // Shift remaining items left
            for (int j = i; j < numItems - 1; j++) {
                itemDB[j] = itemDB[j + 1];
            }
            numItems--;
            printf("Item deleted successfully!\n");
            break;
        }
    }
    if (!found) {
        printf("Item with code %s not found.\n", code);
    }
}

void manageItems() {
    int choice;
    do {
        printf("\n====== Item Database Management ======\n");  
        printf("1. List Items\n");  
        printf("2. Add New Item\n");  
        printf("3. Update Existing Item\n");  
        printf("4. Delete Item\n");  
        printf("5. Back to Main Menu\n");  
        printf("Enter your choice: ");  
        scanf("%d", &choice);
        
        switch (choice) {
            case 1:
                listItems();
                pressEnterToContinue();
                break;
            case 2:
                addItem();
                pressEnterToContinue();
                break;
            case 3:
                updateItem();
                pressEnterToContinue();
                break;
            case 4:
                deleteItem();
                pressEnterToContinue();
                break;
            case 5:
                // Back to main menu
                break;
            default:
                printf("Invalid choice. Try again.\n");
        }
    } while (choice != 5);
}

// ---------------- Option 3: Generate Invoice Summary ----------------

void generateSummary() {
    FILE *fp = fopen("InvoiceSummary.txt", "r");
    if (!fp) {
        printf("No invoice summaries found.\n");
        pressEnterToContinue();
        return;
    }
    // Aggregate totals by customer.
    struct SummaryEntry summary[100];  // assume max 100 distinct customers
    int summaryCount = 0;
    char line[100];
    
    while (fgets(line, sizeof(line), fp)) {
        char cust[50];
        float amt;
        // Line format: customer|total
        if (sscanf(line, "%[^|]|%f", cust, &amt) != 2)
            continue;
        
        int found = 0;
        for (int i = 0; i < summaryCount; i++) {
            if (strcmp(summary[i].name, cust) == 0) {
                summary[i].total += amt;
                found = 1;
                break;
            }
        }
        if (!found) {
            strcpy(summary[summaryCount].name, cust);
            summary[summaryCount].total = amt;
            summaryCount++;
        }
    }
    fclose(fp);
    
    FILE *fp2 = fopen("ConsolidatedSummary.txt", "w");
    if (!fp2) {
        printf("Error creating ConsolidatedSummary.txt\n");
        pressEnterToContinue();
        return;
    }
    
    float grandTotal = 0;
    int totalInvoices = 0;
    
    printf("\n====== Invoice Summary ======\n");  
    printf("Customer Name               Total (INR)\n");  
    printf("---------------------------------------------\n");  
    fprintf(fp2, "Invoice Summary:\n");  
    fprintf(fp2, "Customer Name               Total (INR)\n");  
    fprintf(fp2, "---------------------------------------------\n");  
    
    for (int i = 0; i < summaryCount; i++) {
        printf("%-25s INR%.2f\n", summary[i].name, summary[i].total);
        fprintf(fp2, "%-25s INR%.2f\n", summary[i].name, summary[i].total);
        grandTotal += summary[i].total;
    }
    
    // Count total number of invoices by reading InvoiceSummary.txt
    fp = fopen("InvoiceSummary.txt", "r");
    if (fp) {
        char c;
        while ((c = fgetc(fp)) != EOF) {
            if (c == '\n')
                totalInvoices++;
        }
        fclose(fp);
    }
    
    printf("---------------------------------------------\n");  
    printf("Total Invoices: %d\n", totalInvoices);
    printf("Total Sales: INR%.2f\n", grandTotal);
    fprintf(fp2, "---------------------------------------------\n");  
    fprintf(fp2, "Total Invoices: %d\n", totalInvoices);
    fprintf(fp2, "Total Sales: INR%.2f\n", grandTotal);
    fclose(fp2);
    
    printf("\nConsolidated summary saved to ConsolidatedSummary.txt\n");
    pressEnterToContinue();
}

// ---------------- Main Menu ----------------

int main() {
    int choice;
    // Initialize default item database
    initializeItems();

    do {
        printf("\n====== ADV. RESTAURANT MAIN MENU ======\n");
        printf("1. Create Invoice\n");
        printf("2. Manage Item Database\n");
        printf("3. Generate Invoice Summary\n");
        printf("4. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        clearInputBuffer();

        switch (choice) {
            case 1:
                createInvoice();
                break;
            case 2:
                manageItems();
                break;
            case 3:
                generateSummary();
                break;
            case 4:
                printf("Exiting...\n");
                break;
            default:
                printf("Invalid choice. Try again.\n");
        }
    } while (choice != 4);

    // Free global item database
    free(itemDB);
    return 0;
}
