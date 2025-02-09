# Advanced Restaurant Billing System

This project is an advanced restaurant billing system written in C. It supports dynamic invoice creation for Indian cuisine, complete with a modern bill format and dynamic memory allocation. The system allows users to:

- **Create Invoices:**  
  - Dynamically add items one by one (with the option to list available item codes and names without prices).
  - Use a preloaded item database with special item codes (e.g., for Biryani, Dosa, Paneer Butter Masala, etc.) and prices in INR.
  - Generate a modern invoice format with headers, detailed line items, and footers showing tax and totals.
  - Save each invoice as a text file in an `Invoices` folder and append a summary entry to `InvoiceSummary.txt`.

- **Manage the Item Database:**  
  - List all available items with codes, names, and prices.
  - Add new items, update existing items, or delete items from the database.

- **Generate Invoice Summary:**  
  - Aggregate and display invoice summaries by customer.
  - Save a consolidated summary to `ConsolidatedSummary.txt` including total invoices and total sales.

## Features

- **Dynamic Memory Allocation:**  
  The system uses dynamic memory for handling customer names and order items.

- **Modern Invoice Format:**  
  Clear headers and footers with detailed billing information in INR.

- **Dynamic Item Addition:**  
  Items can be added one by one. When creating an invoice, type `L` (or `l`) at the item code prompt to view a list of available items (codes and names only).

- **File Handling:**  
  Invoices are saved as individual text files in an `Invoices` directory, and a summary of invoices is maintained for further reporting.

## Getting Started

### Prerequisites

- A C compiler (e.g., `gcc`)
- Linux/Unix-based system (or Windows with minor modifications for system commands like `mkdir` and `ls`)

### Compilation

Clone this repository, navigate to the project directory, and compile the code. For example, using `gcc`:

```bash
gcc -o restaurant_billing restaurant_billing.c
