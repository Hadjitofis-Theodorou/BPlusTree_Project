#  B+ Tree Implementation in C

This repository contains a disk-based **B+ Tree** implementation developed for the "Implementation of Database Systems" course (2025-2026). The project utilizes a Block File (BF) library to manage memory pages and disk storage, implementing efficient indexing, insertion, and retrieval of records.

##  Overview

* [cite_start]**Language:** C [cite: 215]
* [cite_start]**System:** Linux / Unix [cite: 217]
* [cite_start]**Data Persistence:** Disk-based (using block-level buffer management) [cite: 113]

##  core Logic: Insertion

The core complexity of this project lies in the **Insertion Algorithm**, which handles dynamic tree resizing and rebalancing.

### 1. The Insertion Process (`bplus_record_insert`)
When a new record is added, the system follows this workflow:

1.  [cite_start]**Traversal:** The function `find_correct_node` starts at the **Root** and follows internal pointers to find the correct **Leaf Node** for the key[cite: 73, 75].
2.  **Leaf Check:**
    * [cite_start]**If space exists:** The record is inserted in sorted order within the leaf[cite: 54].
    * [cite_start]**If full:** The logic triggers a **Split**[cite: 57].

### 2. Handling Splits
We implemented specific strategies to handle overflows at both the Leaf and Index levels:

* **Leaf Split (`split_datanode`):**
    * A new leaf block is allocated.
    * [cite_start]Records are sorted and distributed between the old and new leaf (split point $\approx \frac{N+1}{2}$)[cite: 82].
    * [cite_start]The `next_leaf` pointer is updated to maintain the chain for sequential access[cite: 84].
    * [cite_start]The middle key is "pushed up" to the parent[cite: 85].

* **Index Split (`split_indexnode`):**
    * If the parent node is also full, it splits similarly to the leaf.
    * Keys and pointers are redistributed.
    * A key is promoted recursively up the tree via `insert_into_parent`[cite: 105].

* [cite_start]**Root Splitting:** If the root splits, a new root is created, increasing the tree's height by 1[cite: 103].

## Data Structures

The tree relies on three distinct block types stored on the disk:

1.  [cite_start]**BPlusMeta:** Stores metadata like the `root` block ID, tree `height`, and the file schema[cite: 11, 14].
2.  [cite_start]**BPlusIndexNode (Internal):** Contains navigation keys and pointers to children nodes[cite: 26].
3.  [cite_start]**BPlusDataNode (Leaf):** Contains the actual data records and a `next_leaf` pointer[cite: 16, 21].

## 🛠️ Project Structure

```text
/
[cite_start]├── src/           # Source code (.c) [cite: 190]
[cite_start]├── include/       # Header files (.h) [cite: 188]
[cite_start]├── examples/      # Contains main.c for testing [cite: 191]
[cite_start]├── build/         # Object files [cite: 187]
[cite_start]├── lib/           # Static libraries (libbf.so) [cite: 189]
└── Makefile       # Compilation script
```

##  How to Run

The project includes a `Makefile` that automates compilation and execution.

### 1. Compile
To compile the project and link the libraries:
```bash
make
```
### 2. Run the Example
```bash
make bplus_main_run
```
