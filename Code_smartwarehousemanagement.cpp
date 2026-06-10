#include <iostream>
#include <queue>
#include <stack>
#include <string>
#include <vector>
#include <climits>

using namespace std;

struct ProductNode {
    int id;
    string name;
    ProductNode *left, *right;
    ProductNode(int i, string n) : id(i), name(n), left(NULL), right(NULL) {}
};

class ProductBST {
private:
    ProductNode* root;

    ProductNode* insert(ProductNode* node, int id, string name) {
        if (node == NULL) return new ProductNode(id, name);
        if (id < node->id) node->left = insert(node->left, id, name);
        else if (id > node->id) node->right = insert(node->right, id, name);
        return node;
    }

    void inorder(ProductNode* node) {
        if (node == NULL) return;
        inorder(node->left);
        cout << "ID: " << node->id << " | Product: " << node->name << endl;
        inorder(node->right);
    }

    bool search(ProductNode* node, int id) {
        if (node == NULL) return false;
        if (node->id == id) return true;
        if (id < node->id) return search(node->left, id);
        return search(node->right, id);
    }

public:
    ProductBST() { root = NULL; }
    void addProduct(int id, string name) { root = insert(root, id, name); }
    void showProducts() { inorder(root); cout << endl; }
    bool checkExists(int id) { return search(root, id); }
};


struct Delivery {
    string customer;
    int destIndex;
    string destName;
    int productID; 
};

queue<Delivery> pendingDeliveries;
stack<Delivery> deliveryHistory;   


const int NUM_CITIES = 7;
string cities[NUM_CITIES] = {
    "Islamabad", "Lahore", "Karachi", "Peshawar", "Quetta", "Multan", "Bahawalpur"
};

vector<pair<int, int> > adj[NUM_CITIES];

void initGraph() {
    adj[0].push_back(make_pair(1, 380)); adj[0].push_back(make_pair(3, 180));
    adj[1].push_back(make_pair(0, 380)); adj[1].push_back(make_pair(5, 340));
    adj[2].push_back(make_pair(4, 680)); adj[2].push_back(make_pair(5, 900));
    adj[3].push_back(make_pair(0, 180));
    adj[4].push_back(make_pair(2, 680)); adj[4].push_back(make_pair(5, 420));
    adj[5].push_back(make_pair(1, 340)); adj[5].push_back(make_pair(2, 900)); adj[5].push_back(make_pair(4, 420)); adj[5].push_back(make_pair(6, 100));
    adj[6].push_back(make_pair(5, 100));
}

void showShortestPath(int destination) {
    if (destination == 0) {
        cout << "Destination is Islamabad HQ. Distance: 0 km\n";
        return;
    }

    vector<int> dist(NUM_CITIES, INT_MAX);
    vector<int> parent(NUM_CITIES, -1);
    
    priority_queue<pair<int, int>, vector<pair<int, int> >, greater<pair<int, int> > > pq;

    dist[0] = 0;
    pq.push(make_pair(0, 0)); 

    while (!pq.empty()) {
        int d = pq.top().first;
        int u = pq.top().second;
        pq.pop();

        if (d != dist[u]) continue;

        for (size_t i = 0; i < adj[u].size(); i++) {
            int v = adj[u][i].first;
            int w = adj[u][i].second;
            
            if (dist[u] != INT_MAX && dist[u] + w < dist[v]) {
                dist[v] = dist[u] + w;
                parent[v] = u; 
                pq.push(make_pair(dist[v], v));
            }
        }
    }

    vector<int> path;
    int curr = destination;
    while (curr != -1) {
        path.push_back(curr);
        curr = parent[curr];
    }

    cout << "\n=> SHORTEST ROUTE FOUND:\nPath: ";
    for (int i = path.size() - 1; i >= 0; i--) {
        cout << cities[path[i]];
        if (i != 0) cout << " -> ";
    }
    cout << "\nTotal Distance: " << dist[destination] << " km\n";
}

void handleAddProduct(ProductBST& bst) {
    int id; string name;
    cout << "Enter Product ID: "; cin >> id;
    cout << "Enter Product Name: "; cin >> name;
    bst.addProduct(id, name);
    cout << "=> Product successfully added to inventory.\n";
}

void setDelivery(ProductBST& bst) {
    Delivery d;
    cout << "Enter Customer Name: "; cin >> d.customer;
    cout << "Enter Product ID: "; cin >> d.productID;

    if (!bst.checkExists(d.productID)) {
        cout << "=> ERROR: Product ID " << d.productID << " not found in inventory!\n";
        return; 
    }

    cout << "\nDestinations: 1.Lahore 2.Karachi 3.Peshawar 4.Quetta 5.Multan 6.Bahawalpur\n";
    cout << "Enter Destination Number (1-6): "; cin >> d.destIndex;

    if (d.destIndex >= 1 && d.destIndex <= 6) {
        d.destName = cities[d.destIndex];
        pendingDeliveries.push(d);
        cout << "\n=> Order Confirmed! Product " << d.productID << " reserved for " << d.customer << ".\n";
        showShortestPath(d.destIndex); 
    } else {
        cout << "=> ERROR: Invalid destination.\n";
    }
}

void processDelivery() {
    if (!pendingDeliveries.empty()) {
        Delivery d = pendingDeliveries.front();
        pendingDeliveries.pop();
        deliveryHistory.push(d);
        cout << "=> Dispatched Product " << d.productID << " to " << d.customer << " in " << d.destName << ".\n";
    } else {
        cout << "=> No pending deliveries in Queue.\n";
    }
}

void showHistory() {
    if (deliveryHistory.empty()) {
        cout << "=> History log is empty.\n";
    } else {
        cout << "\n--- COMPLETED DELIVERIES (LIFO LOG) ---\n";
        stack<Delivery> tempStack = deliveryHistory; 
        while (!tempStack.empty()) {
            Delivery d = tempStack.top();
            tempStack.pop();
            cout << "Customer: " << d.customer << " | Product ID: " << d.productID << " | Shipped to: " << d.destName << endl;
        }
    }
}

int main() {
    initGraph(); 
    ProductBST bst;
    int choice;

    do {
        cout << "\n--- SMART WAREHOUSE (ISLAMABAD HQ) ---\n";
        cout << "1. Add Product (BST)\n";
        cout << "2. Show Inventory (BST)\n";
        cout << "3. Set New Delivery (Queue & Shortest Path)\n";
        cout << "4. Process Delivery (Queue -> Stack)\n";
        cout << "5. Show Delivery History (Stack)\n";
        cout << "0. Exit\n";
        cout << "Choice: "; cin >> choice;

        switch (choice) {
            case 1: handleAddProduct(bst); break;
            case 2: cout << "\n--- CURRENT INVENTORY ---\n"; bst.showProducts(); break;
            case 3: setDelivery(bst); break;
            case 4: processDelivery(); break;
            case 5: showHistory(); break;
            case 0: cout << "Exiting system...\n"; break;
            default: cout << "Invalid choice. Please choose options 0 to 5.\n"; break;
        }
    } while (choice != 0);

    return 0;
}
