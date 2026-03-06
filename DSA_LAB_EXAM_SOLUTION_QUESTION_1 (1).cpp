#include <iostream>
using namespace std;
const int MAXN = 2000;
const int M = 10;
const int EMPTY_VAL   = -999999;   // marks an empty slot in hash table
const int DELETED_VAL =  999999;   // marks a tombstone slot in hash table
/* -------------------- Request -------------------- */
class Request {
public:
    int reqID;
    int priority;
    int time;

    Request() {
        reqID = 0;
        priority = 0;
        time = 0;
    }
};
/* -------------------- Insertion Sort -------------------- */
class Sorter {
public:
    static int lessReq(const Request& a, const Request& b) {
        if (a.priority != b.priority) return a.priority < b.priority;
        return a.time < b.time;
    }

    static void insertionSort(Request a[], int n) {
        int i, j;
        for (i = 1; i < n; i++) {
            Request key = a[i];
            j = i - 1;
            while (j >= 0 && lessReq(key, a[j])) {
                a[j + 1] = a[j];
                j--;
            }
            a[j + 1] = key;
        }
    }
};
/* -------------------- Simple Stack (array) -------------------- */
class SimpleStack {
private:
    int arr[MAXN];
    int top;

public:
    SimpleStack() { top = -1; }

    int empty() const { return top < 0; }

    void push(int x) {
        if (top < MAXN - 1) {
            top++;
            arr[top] = x;
        }
    }

    int pop() {
        int x = arr[top];
        top--;
        return x;
    }

    void printTopToBottom() const {
        if (empty()) {
            cout << "Empty";
            return;
        }
        int i;
        for (i = top; i >= 0; i--) {
            if (i != top) cout << " ";
            cout << arr[i];
        }
    }
};
/* -------------------- Simple Queue (circular array) -------------------- */
class SimpleQueue {
private:
    int arr[MAXN];
    int front;
    int rear;
    int count;
public:
    SimpleQueue() {
        front = 0;
        rear = 0;
        count = 0;
    }

    int empty() const { return count == 0; }

    void push(int x) {
        if (count < MAXN) {
            arr[rear] = x;
            rear = (rear + 1) % MAXN;
            count++;
        }
    }

    int pop() {
        int x = arr[front];
        front = (front + 1) % MAXN;
        count--;
        return x;
    }

    void printFrontToRear() const {
        if (empty()) {
            cout << "Empty";
            return;
        }
        int i, idx;
        for (i = 0; i < count; i++) {
            idx = (front + i) % MAXN;
            if (i > 0) cout << " ";
            cout << arr[idx];
        }
    }
};
/* -------------------- Processing Engine -------------------- */
class ProcessingEngine {
private:
    SimpleStack S;
    SimpleQueue Q;
public:
    void loadFromSorted(const Request reqs[], int n) {
        int i;
        for (i = 0; i < n; i++) {
            if (reqs[i].priority == 1) S.push(reqs[i].reqID);
            else Q.push(reqs[i].reqID);
        }
    }

    int processAll(int processed[], int maxOut) {
        cout << "PROCESSING TRACE :\n";
        int cnt = 0;

        while ((!S.empty() || !Q.empty()) && cnt < maxOut) {
            int id;
            if (!S.empty()) id = S.pop();
            else id = Q.pop();

            processed[cnt] = id;
            cnt++;

            cout << "Process : " << id << " | Stack : ";
            S.printTopToBottom();
            cout << " | Queue : ";
            Q.printFrontToRear();
            cout << "\n";
        }
        return cnt;
    }
};
/* -------------------- Doubly Linked List -------------------- */
class DLLNode {
public:
    int val;
    DLLNode* prev;
    DLLNode* next;

    DLLNode(int x) {
        val = x;
        prev = NULL;
        next = NULL;
    }
};
class DoublyLinkedList {
private:
    DLLNode* head;
    DLLNode* tail;
public:
    DoublyLinkedList() {
        head = NULL;
        tail = NULL;
    }

    ~DoublyLinkedList() { destroy(); }

    void insertTail(int x) {
        DLLNode* node = new DLLNode(x);

        if (tail == NULL) {
            head = node;
            tail = node;
            return;
        }
        tail->next = node;
        node->prev = tail;
        tail = node;
    }

    int deleteByID(int x) {
        DLLNode* cur = head;
        while (cur != NULL && cur->val != x) cur = cur->next;
        if (cur == NULL) return 0;

        if (cur->prev != NULL) cur->prev->next = cur->next;
        else head = cur->next;

        if (cur->next != NULL) cur->next->prev = cur->prev;
        else tail = cur->prev;

        delete cur;
        return 1;
    }

    void displayForward() const {
        if (head == NULL) {
            cout << "Empty\n";
            return;
        }
        DLLNode* cur = head;
        while (cur != NULL) {
            cout << cur->val;
            if (cur->next != NULL) cout << " <-> ";
            cur = cur->next;
        }
        cout << "\n";
    }

    void displayBackward() const {
        if (tail == NULL) {
            cout << "Empty\n";
            return;
        }
        DLLNode* cur = tail;
        while (cur != NULL) {
            cout << cur->val;
            if (cur->prev != NULL) cout << " <-> ";
            cur = cur->prev;
        }
        cout << "\n";
    }

    void destroy() {
        DLLNode* cur = head;
        while (cur != NULL) {
            DLLNode* nxt = cur->next;
            delete cur;
            cur = nxt;
        }
        head = NULL;
        tail = NULL;
    }
};
/* -------------------- Hash Table (Linear Probing + Tombstone) -------------------- */
class SearchResult {
public:
    int idx;
    int probes;
};
class HashTable {
private:
    int table[M];

    int h(int k) const {
        int r = k % M;
        if (r < 0) r += M;
        return r;
    }
public:
    HashTable() { init(); }

    void init() {
        int i;
        for (i = 0; i < M; i++) table[i] = EMPTY_VAL;
    }

    void insertKey(int k) {
        int start = h(k);
        int i;
        for (i = 0; i < M; i++) {
            int idx = (start + i) % M;
            if (table[idx] == k) return;
            if (table[idx] == EMPTY_VAL || table[idx] == DELETED_VAL) {
                table[idx] = k;
                return;
            }
        }
    }
    SearchResult searchKey(int k) const {
        SearchResult res;
        int start = h(k);
        int probes = 0;
        int i;

        for (i = 0; i < M; i++) {
            int idx = (start + i) % M;
            probes++;

            if (table[idx] == EMPTY_VAL) {
                res.idx = -1;
                res.probes = probes;
                return res;
            }
            if (table[idx] == k) {
                res.idx = idx;
                res.probes = probes;
                return res;
            }
        }
        res.idx = -1;
        res.probes = probes;
        return res;
    }
    int deleteKey(int k) {
        SearchResult r = searchKey(k);
        if (r.idx != -1) {
            table[r.idx] = DELETED_VAL;
            return 1;
        }
        return 0;
    }
    void printFinal() const {
        cout << "FINAL HASH TABLE :\n";
        int i;
        for (i = 0; i < M; i++) {
            cout << i << ": ";
            if (table[i] == EMPTY_VAL) cout << "EMPTY";
            else if (table[i] == DELETED_VAL) cout << "DELETED";
            else cout << table[i];
            cout << "\n";
        }
    }
};
/* -------------------- BST -------------------- */
class BSTNode {
public:
    int val;
    BSTNode* left;
    BSTNode* right;

    BSTNode(int x) {
        val = x;
        left = NULL;
        right = NULL;
    }
};
class BST {
private:
    BSTNode* root;

    BSTNode* insertRec(BSTNode* node, int x) {
        if (node == NULL) return new BSTNode(x);
        if (x < node->val) node->left = insertRec(node->left, x);
        else if (x > node->val) node->right = insertRec(node->right, x);
        return node;
    }

    void destroyRec(BSTNode* node) {
        if (node == NULL) return;
        destroyRec(node->left);
        destroyRec(node->right);
        delete node;
    }
    void preorderFill(BSTNode* node, int arr[], int& c) const {
        if (node == NULL) return;
        arr[c++] = node->val;
        preorderFill(node->left, arr, c);
        preorderFill(node->right, arr, c);
    }
    void inorderFill(BSTNode* node, int arr[], int& c) const {
        if (node == NULL) return;
        inorderFill(node->left, arr, c);
        arr[c++] = node->val;
        inorderFill(node->right, arr, c);
    }
    void postorderFill(BSTNode* node, int arr[], int& c) const {
        if (node == NULL) return;
        postorderFill(node->left, arr, c);
        postorderFill(node->right, arr, c);
        arr[c++] = node->val;
    }

    void printList(const int arr[], int c) const {
        int i;
        for (i = 0; i < c; i++) {
            if (i > 0) cout << " ";
            cout << arr[i];
        }
    }
public:
    BST() { root = NULL; }

    ~BST() { destroyRec(root); }

    void insert(int x) { root = insertRec(root, x); }

    void printTraversals() const {
        cout << "BST TRAVERSALS :\n";

        int pre[MAXN], in[MAXN], post[MAXN];
        int c1 = 0, c2 = 0, c3 = 0;

        preorderFill(root, pre, c1);
        inorderFill(root, in, c2);
        postorderFill(root, post, c3);

        cout << "Preorder : ";
        printList(pre, c1);
        cout << "\n";

        cout << "Inorder : ";
        printList(in, c2);
        cout << "\n";

        cout << "Postorder : ";
        printList(post, c3);
        cout << "\n";
    }
};
/* -------------------- MAIN -------------------- */
int main() {
    int n, i;
    cout << "Enter n :\n";
    cin >> n;
    cout << "Enter " << n << " lines ( reqID priority time ) :\n";
    Request reqs[MAXN];
    for (i = 0; i < n; i++) {
        cin >> reqs[i].reqID >> reqs[i].priority >> reqs[i].time;
    }
    /* Part A */
    Sorter::insertionSort(reqs, n);
    cout << "SORTED REQUESTS :\n";
    for (i = 0; i < n; i++) {
        cout << reqs[i].reqID << " " << reqs[i].priority << " " << reqs[i].time << "\n";
    }
    /* Part B */
    ProcessingEngine engine;
    engine.loadFromSorted(reqs, n);
    int processed[MAXN];
    int processedCount = engine.processAll(processed, MAXN);
    /* Part C */
    int reqID_del;
    cout << "Enter reqID_del ( for DLL deletion ) :\n";
    cin >> reqID_del;
    DoublyLinkedList dll;
    for (i = 0; i < processedCount; i++) dll.insertTail(processed[i]);
    cout << "DLL FORWARD :\n";
    dll.displayForward();
    cout << "DLL BACKWARD :\n";
    dll.displayBackward();
    dll.deleteByID(reqID_del);
    cout << "DLL AFTER DELETION :\n";
    dll.displayForward();
    /* Part D */
    int q;
    cout << "Enter q ( number of hash searches ):\n";
    cin >> q;
    cout << "Enter " << q << " search keys :\n";
    int keys[MAXN];
    for (i = 0; i < q; i++) cin >> keys[i];
    int kdel;
    cout << "Enter kdel ( delete from hash table ) :\n";
    cin >> kdel;
    HashTable ht;
    for (i = 0; i < processedCount; i++) ht.insertKey(processed[i]);
    cout << "HASH SEARCH RESULTS :\n";
    for (i = 0; i < q; i++) {
        SearchResult r = ht.searchKey(keys[i]);
        if (r.idx != -1) {
            cout << "Search " << keys[i] << ": Found at Index " << r.idx
                 << " , Probes : " << r.probes << "\n";
        } else {
            cout << "Search " << keys[i] << ": Not Found"
                 << " , Probes : " << r.probes << "\n";
        }
    }
    if (!ht.deleteKey(kdel)) {
        cout << "Not Found\n";
    }
    ht.printFinal();
    /* Part E */
    BST bst;
    for (i = 0; i < processedCount; i++) bst.insert(processed[i]);
    bst.printTraversals();
    return 0;
}
