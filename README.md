
## Δομές

### 1. BPlusMeta — Μεταδεδομένα Αρχείου B+ Δέντρου
typedef struct BPlusMeta {
    int file_type;
    int root;
    int height;
    TableSchema schema;
} BPlusMeta;

-   file_type: σταθερή τιμή (εμείς βάλαμε 2004) για έλεγχο ότι το αρχείο είναι έγκυρο B+ tree.
-   root: block ID της ρίζας.

-   height: ύψος του δέντρου.

-   schema: το schema

### 2. BPlusDataNode —Μπλόκ Δεδομένων
typedef struct BPlusDataNode {
    int is_leaf;
    int num_keys;
    int parent;
    int next_leaf;
    Record record[MAX_DATA_KEYS];
} BPlusDataNode;


-   Αναπαριστά leaf node που περιέχει τις εγγραφές.

-   is_leaf: βοηθά γεννικά για να βρίσκουμε γρήγορα αν είναι leaf node σίγουρα

-   num_keys: πλήθος εγγραφών που βρίσκονται στο node

-   parent: block ID γονέα

-   next_leaf: block ID του επόμενου leaf .

-   record[]: πίνακας εγγραφών σταθερού μεγέθους που υπολογίζεται με βάση το block size.

το MAX_DATA_KEYS είναι ο μέγιστος αριθμός records που χωράει ένα μπλοκ,(blocksize-4integers ως μεταδεδομένα του μπλο)/μέγεθος μιας εγγραφης

### 3. BPlusIndexNode — Μπλοκ ευρετηρίου
typedef struct BPlusIndexNode {
    int is_leaf;
    int num_keys;
    int parent;
    int keys[MAX_INDEX_KEYS];
    int pointers[MAX_INDEX_KEYS + 1];
} BPlusIndexNode;

-   is_leaf: παραμένει για ομοιομορφία
-   num_keys: πλήθος κλειδιών στον κόμβο
-   parent: block ID του γονέα.
-   keys[]: κλειδιά
-   pointers[]: δείκτες σε child blocks, πάντα num_keys + 1.
Το MAX_INDEX_KEYS υπολογίστικες ομοίως

