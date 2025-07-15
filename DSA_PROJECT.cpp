#include <iostream>
#include <string>
using namespace std;


struct UserNode;

struct FriendNode {
    UserNode* user; 
    FriendNode* next;
};

struct MessageNode {
    UserNode* sender; 
    string text;
    MessageNode* next;
};

struct UserNode {
    string name, id, city, interest, institution;
    UserNode* left;
    UserNode* right;
    FriendNode* friends; 
    FriendNode* pendingRequests; 
    MessageNode* messages; 
};

struct GroupMemberNode {
    UserNode* user;
    GroupMemberNode* next;
};

struct GroupNode {
    string groupName;
    GroupMemberNode* members;
    GroupNode* next;
};

class Profile {
    UserNode* root; // BST root
    GroupNode* groupHead; // linked list of groups

public:
    Profile() : root(NULL), groupHead(NULL) {}
    ~Profile() { clearUsers(root); clearGroups(); }

    // BST insert
    UserNode* addUser(UserNode* current, UserNode* newUser) {
        if (!current) return newUser;
        if (newUser->id < current->id)
            current->left = addUser(current->left, newUser);
        else if (newUser->id > current->id)
            current->right = addUser(current->right, newUser);
        return current;
    }

    void makeProfile() {
        UserNode* newUser = new UserNode;
        newUser->left = newUser->right = NULL;
        newUser->friends = NULL;
        newUser->pendingRequests = NULL;
        newUser->messages = NULL;
        
        cout << "Enter your name: "; getline(cin, newUser->name);
        if(newUser->name.empty()){ cout << "Name cannot be empty.\n"; delete newUser; return; }

        cout << "Enter your id/Number: "; getline(cin, newUser->id);
        if(newUser->id.empty()){ cout << "ID cannot be empty.\n"; delete newUser; return; }

        cout << "Enter your city: "; getline(cin, newUser->city);
        cout << "Enter your interests: "; getline(cin, newUser->interest);
        cout << "Enter Your institution: "; getline(cin, newUser->institution);
        
        if (findUser(root, newUser->id)) {
            cout << "User with this ID already exists!\n";
            delete newUser;
            return;
        }
        root = addUser(root, newUser);
        cout << "Profile created successfully!\n";
    }

    UserNode* findUser(UserNode* current, const string& id) {
        if (!current) return NULL;
        if (id == current->id) return current;
        if (id < current->id) return findUser(current->left, id);
        return findUser(current->right, id);
    }

    void sendFriendRequest(const string& senderId, const string& receiverId) {
        if (senderId == receiverId) {
            cout << "You cannot send a friend request to yourself.\n";
            return;
        }
        UserNode* sender = findUser(root, senderId);
        UserNode* receiver = findUser(root, receiverId);
        if (!sender || !receiver) {
            cout << "Invalid user IDs. Friend request not sent.\n";
            return;
        }
        if (areFriends(sender, receiver)) {
            cout << "You are already friends.\n";
            return;
        }
        if (hasPendingRequest(receiver, sender) || hasPendingRequest(sender, receiver)) {
            cout << "A friend request is already pending between you two.\n";
            return;
        }
        // Add a pending request to the receiver's list
        FriendNode* req = new FriendNode;
        req->user = sender;
        req->next = receiver->pendingRequests;
        receiver->pendingRequests = req;
        cout << "Friend request sent from " << sender->name << " to " << receiver->name << ".\n";
    }

    void acceptFriendRequest(const string& userId, const string& senderId) {
        UserNode* user = findUser(root, userId);
        UserNode* sender = findUser(root, senderId);
        if (!user || !sender) {
            cout << "Invalid user IDs. Friend request not accepted.\n";
            return;
        }
        // Find and remove pending request from the user's pending list
        FriendNode **ptr_to_req = &user->pendingRequests;
        FriendNode *req_node = user->pendingRequests;

        while(req_node != NULL){
            if(req_node->user == sender){
                *ptr_to_req = req_node->next; // Unlink the node
                delete req_node; // Free the memory for the request node

                // Add each other as friends
                addFriend(user, sender);
                addFriend(sender, user);
                cout << sender->name << " and " << user->name << " are now friends.\n";
                return;
            }
            ptr_to_req = &req_node->next;
            req_node = req_node->next;
        }
        cout << "No pending friend request from " << sender->name << ".\n";
    }

    void addFriend(UserNode* user, UserNode* friendUser) {
        if (areFriends(user, friendUser)) return;
        FriendNode* f = new FriendNode;
        f->user = friendUser;
        f->next = user->friends;
        user->friends = f;
    }

    bool areFriends(UserNode* a, UserNode* b) {
        for (FriendNode* curr = a->friends; curr; curr = curr->next) {
            if (curr->user == b) return true;
        }
        return false;
    }

    bool hasPendingRequest(UserNode* receiver, UserNode* sender) {
        for (FriendNode* curr = receiver->pendingRequests; curr; curr = curr->next) {
            if (curr->user == sender) return true;
        }
        return false;
    }

    void sendMessage(const string& senderId, const string& receiverId, const string& message) {
        UserNode* sender = findUser(root, senderId);
        UserNode* receiver = findUser(root, receiverId);
        if (!sender || !receiver) {
            cout << "Invalid user IDs. Message not sent.\n";
            return;
        }
        if (!areFriends(sender, receiver)) {
            cout << "You are not friends. Add as friends first before messaging.\n";
            return;
        }
        MessageNode* msg = new MessageNode;
        msg->sender = sender;
        msg->text = message;
        msg->next = receiver->messages;
        receiver->messages = msg;
        cout << "Message sent from " << sender->name << " to " << receiver->name << ".\n";
    }

    void readMessages(const string& userId) {
        UserNode* user = findUser(root, userId);
        if (!user) {
            cout << "Invalid user ID.\n";
            return;
        }
        cout << "Messages for user " << user->name << ":\n";
        MessageNode* curr = user->messages;
        if (!curr) cout << "No messages.\n";
        while (curr) {
            cout << "From: " << curr->sender->name << " (" << curr->sender->id << ") - Message: " << curr->text << endl;
            curr = curr->next;
        }
    }

    void createGroup(const string& groupName) {
        if (findGroup(groupName)) {
            cout << "Group already exists!\n";
            return;
        }
        GroupNode* g = new GroupNode;
        g->groupName = groupName;
        g->members = NULL;
        g->next = groupHead;
        groupHead = g;
        cout << "Group " << groupName << " created.\n";
    }

    void joinGroup(const string& userId, const string& groupName) {
        UserNode* user = findUser(root, userId);
        if (!user) {
            cout << "Invalid user ID. Cannot join the group.\n";
            return;
        }
        GroupNode* group = findGroup(groupName);
        if (!group) {
            cout << "Group not found. Create the group first.\n";
            return;
        }
        if (isGroupMember(group, user)) {
            cout << "User already in group.\n";
            return;
        }
        GroupMemberNode* m = new GroupMemberNode;
        m->user = user;
        m->next = group->members;
        group->members = m;
        cout << user->name << " joined the group " << groupName << ".\n";
    }

    void listGroupMembers(const string& groupName) {
        GroupNode* group = findGroup(groupName);
        if (!group) {
            cout << "Group not found.\n";
            return;
        }
        cout << "Members of the group " << groupName << ":\n";
        GroupMemberNode* curr = group->members;
        if (!curr) cout << "No members.\n";
        while (curr) {
            cout << curr->user->name << " (" << curr->user->id << ")\n";
            curr = curr->next;
        }
    }

    void showMutualFriends(const string& user1Id, const string& user2Id) {
        UserNode* u1 = findUser(root, user1Id);
        UserNode* u2 = findUser(root, user2Id);
        if (!u1 || !u2) {
            cout << "Invalid user IDs.\n";
            return;
        }
        cout << "Mutual friends between " << u1->name << " and " << u2->name << ":\n";
        bool found = false;
        for (FriendNode* f1 = u1->friends; f1; f1 = f1->next) {
            for (FriendNode* f2 = u2->friends; f2; f2 = f2->next) {
                if (f1->user == f2->user) {
                    cout << f1->user->name << " (" << f1->user->id << ")\n";
                    found = true;
                }
            }
        }
        if (!found) cout << "No mutual friends.\n";
    }

    void suggestFriends(const string& userId) {
        UserNode* user = findUser(root, userId);
        if (!user) {
            cout << "Invalid user ID.\n";
            return;
        }
        cout << "Friend suggestions for " << user->name << ":\n";
        bool found = false;
        suggestFriendsRec(root, user, found);
        if (!found) cout << "No suggestions.\n";
    }
    void suggestFriendsRec(UserNode* current, UserNode* user, bool& found) {
        if (!current) return;
        if (current != user && !areFriends(user, current) && !hasPendingRequest(user, current) && !hasPendingRequest(current, user)) {
            string reason = "";
            if (current->city == user->city && !current->city.empty()) reason += "[Same City] ";
            if (current->interest == user->interest && !current->interest.empty()) reason += "[Same Interest] ";
            if (current->institution == user->institution && !current->institution.empty()) reason += "[Same Institution]";

            if (!reason.empty()) {
                 cout << "Name: " << current->name << ", ID: " << current->id << " - Reason: " << reason << endl;
                 found = true;
            }
        }
        suggestFriendsRec(current->left, user, found);
        suggestFriendsRec(current->right, user, found);
    }

    void sendFriendRequestToGroup(const string& userId, const string& groupName) {
        UserNode* user = findUser(root, userId);
        GroupNode* group = findGroup(groupName);
        if (!user || !group) {
            cout << "Invalid user ID or group name.\n";
            return;
        }
        bool any = false;
        for (GroupMemberNode* m = group->members; m; m = m->next) {
            if (m->user != user && !areFriends(user, m->user) && !hasPendingRequest(m->user, user) && !hasPendingRequest(user, m->user)) {
                sendFriendRequest(userId, m->user->id);
                any = true;
            }
        }
        if (!any) cout << "No new friend requests sent.\n";
    }

    void listAllUsers() {
        cout << "All users:\n";
        listAllUsersRec(root);
    }
    void listAllUsersRec(UserNode* current) {
        if (!current) return;
        listAllUsersRec(current->left);
        cout << current->name << " (" << current->id << ")\n";
        listAllUsersRec(current->right);
    }

    void listFriends(const string& userId) {
        UserNode* user = findUser(root, userId);
        if (!user) {
            cout << "Invalid user ID.\n";
            return;
        }
        cout << "Friends of " << user->name << ":\n";
        bool found = false;
        for (FriendNode* f = user->friends; f; f = f->next) {
            cout << f->user->name << " (" << f->user->id << ")\n";
            found = true;
        }
        if (!found) cout << "No friends.\n";
    }

    void listPendingRequests(const string& userId) {
        UserNode* user = findUser(root, userId);
        if (!user) {
            cout << "Invalid user ID.\n";
            return;
        }
        cout << "Pending friend requests for " << user->name << ":\n";
        bool found = false;
        for (FriendNode* req = user->pendingRequests; req; req = req->next) {
            cout << req->user->name << " (" << req->user->id << ")\n";
            found = true;
        }
        if (!found) cout << "No pending requests.\n";
    }

    void editProfile(const string& userId) {
        UserNode* user = findUser(root, userId);
        if (!user) {
            cout << "Invalid user ID.\n";
            return;
        }

        int choice;
        string input;
        while (true) {
            cout << "\nEdit Profile for " << user->name << ":\n";
            cout << "1. Edit Name\n";
            cout << "2. Edit City\n";
            cout << "3. Edit Interests\n";
            cout << "4. Edit Institution\n";
            cout << "0. Back to Main Menu\n";
            cout << "Enter your choice: ";
            cin >> choice;
            if (!cin) {
                cout << "Invalid input.\n";
                cin.clear();
                cin.ignore(10000, '\n');
                continue;
            }
            cin.ignore();

            if (choice == 0) break;

            cout << "Enter new value: ";
            getline(cin, input);
            if (input.empty()) {
                cout << "Input cannot be empty.\n";
                continue;
            }

            switch (choice) {
                case 1: user->name = input; cout << "Name updated.\n"; break;
                case 2: user->city = input; cout << "City updated.\n"; break;
                case 3: user->interest = input; cout << "Interests updated.\n"; break;
                case 4: user->institution = input; cout << "Institution updated.\n"; break;
                default: cout << "Invalid choice.\n";
            }
        }
    }

    void deleteFriend(const string& userId, const string& friendId) {
        if (userId == friendId) {
            cout << "You cannot unfriend yourself.\n";
            return;
        }
        UserNode* user = findUser(root, userId);
        UserNode* friendUser = findUser(root, friendId);
        if (!user || !friendUser) {
            cout << "Invalid user ID(s).\n";
            return;
        }
        if (!areFriends(user, friendUser)) {
            cout << "You are not friends with this user.\n";
            return;
        }

        removeFriend(user, friendUser);
        removeFriend(friendUser, user);
        cout << "You are no longer friends with " << friendUser->name << ".\n";
    }

    void rejectFriendRequest(const string& userId, const string& senderId) {
        UserNode* user = findUser(root, userId);
        UserNode* sender = findUser(root, senderId);
        if (!user || !sender) {
            cout << "Invalid user IDs.\n";
            return;
        }

        FriendNode** pp = &user->pendingRequests;
        while (*pp) {
            if ((*pp)->user == sender) {
                FriendNode* temp = *pp;
                *pp = (*pp)->next;
                delete temp;
                cout << "Friend request from " << sender->name << " rejected.\n";
                return;
            }
            pp = &(*pp)->next;
        }
        cout << "No pending request from " << sender->name << " found.\n";
    }

    void sendGroupMessage(const string& senderId, const string& groupName, const string& message) {
        UserNode* sender = findUser(root, senderId);
        if (!sender) {
            cout << "Invalid sender ID.\n";
            return;
        }
        GroupNode* group = findGroup(groupName);
        if (!group) {
            cout << "Group not found.\n";
            return;
        }
        if (!isGroupMember(group, sender)) {
            cout << "You are not a member of this group.\n";
            return;
        }

        int count = 0;
        for (GroupMemberNode* member = group->members; member; member = member->next) {
            if (member->user != sender) {
                MessageNode* msg = new MessageNode;
                msg->sender = sender;
                msg->text = message;
                msg->next = member->user->messages;
                member->user->messages = msg;
                count++;
            }
        }
        cout << "Message sent to " << count << " members of group " << groupName << ".\n";
    }

    void viewUserProfile(const string& userId) {
        UserNode* user = findUser(root, userId);
        if (!user) {
            cout << "User not found.\n";
            return;
        }
        cout << "\n--- User Profile ---\n";
        cout << "Name: " << user->name << "\n";
        cout << "ID: " << user->id << "\n";
        cout << "City: " << user->city << "\n";
        cout << "Interests: " << user->interest << "\n";
        cout << "Institution: " << user->institution << "\n";
        cout << "--------------------\n";
        listFriends(userId);
        cout << "--------------------\n";
    }

private:
    void removeFriend(UserNode* user, UserNode* friendToRemove) {
        FriendNode** pp = &user->friends;
        while (*pp) {
            if ((*pp)->user == friendToRemove) {
                FriendNode* temp = *pp;
                *pp = (*pp)->next;
                delete temp;
                return;
            }
            pp = &(*pp)->next;
        }
    }
    void clearUsers(UserNode* node) {
        if (!node) return;
        clearUsers(node->left);
        clearUsers(node->right);
        // Delete friends
        FriendNode* f = node->friends;
        while (f) {
            FriendNode* tmp = f;
            f = f->next;
            delete tmp;
        }
        // Delete pending requests
        FriendNode* p = node->pendingRequests;
        while (p) {
            FriendNode* tmp = p;
            p = p->next;
            delete tmp;
        }
        // Delete messages
        MessageNode* m = node->messages;
        while (m) {
            MessageNode* tmp = m;
            m = m->next;
            delete tmp;
        }
        delete node;
    }
    void clearGroups() {
        GroupNode* g = groupHead;
        while (g) {
            GroupMemberNode* m = g->members;
            while (m) {
                GroupMemberNode* tmp = m;
                m = m->next;
                delete tmp;
            }
            GroupNode* tmpg = g;
            g = g->next;
            delete tmpg;
        }
    }
    GroupNode* findGroup(const string& groupName) {
        GroupNode* curr = groupHead;
        while (curr) {
            if (curr->groupName == groupName) return curr;
            curr = curr->next;
        }
        return NULL;
    }
    bool isGroupMember(GroupNode* group, UserNode* user) {
        GroupMemberNode* curr = group->members;
        while (curr) {
            if (curr->user == user) return true;
            curr = curr->next;
        }
        return false;
    }
};

int main() {
    Profile profile;
    int choice;
    while (1) {
        cout << "\nMenu:\n";
        cout << "1. Create Profile\n";
        cout << "2. Create Group\n";
        cout << "3. Send Friend Request\n";
        cout << "4. Accept Friend Request\n";
        cout << "5. Send Message\n";
        cout << "6. Read Messages\n";
        cout << "7. List Group Members\n";
        cout << "8. Show Mutual Friends\n";
        cout << "9. Send Friend Request to Group Members\n";
        cout << "10. Suggest Friends\n";
        cout << "11. List All Users\n";
        cout << "12. List Friends\n";
        cout << "13. List Pending Friend Requests\n";
        cout << "14. Edit Profile\n";
        cout << "15. Delete Friend\n";
        cout << "16. Reject Friend Request\n";
        cout << "17. Send Group Message\n";
        cout << "18. View User Profile\n";
        cout << "0. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;
        if (!(cin)) {
            cout << "Invalid input. Please enter a number.\n";
            cin.clear();
            cin.ignore(10000, '\n');
            continue;
        }
        cin.ignore();
        if (choice == 1) {
            profile.makeProfile();
        } else if (choice == 2) {
            string groupName;
            cout << "Enter group name: ";
            getline(cin, groupName);
            profile.createGroup(groupName);
        } else if (choice == 3) {
            string senderId, receiverId;
            cout << "Enter sender ID: "; getline(cin, senderId);
            cout << "Enter receiver ID: "; getline(cin, receiverId);
            profile.sendFriendRequest(senderId, receiverId);
        } else if (choice == 4) {
            string userId, senderId;
            cout << "Enter your user ID: "; getline(cin, userId);
            cout << "Enter sender ID: "; getline(cin, senderId);
            profile.acceptFriendRequest(userId, senderId);
        } else if (choice == 5) {
            string senderId, receiverId, message;
            cout << "Enter sender ID: "; getline(cin, senderId);
            cout << "Enter receiver ID: "; getline(cin, receiverId);
            cout << "Enter message: "; getline(cin, message);
            profile.sendMessage(senderId, receiverId, message);
        } else if (choice == 6) {
            string userId;
            cout << "Enter user ID: "; getline(cin, userId);
            profile.readMessages(userId);
        } else if (choice == 7) {
            string groupName;
            cout << "Enter group name: "; getline(cin, groupName);
            profile.listGroupMembers(groupName);
        } else if (choice == 8) {
            string user1Id, user2Id;
            cout << "Enter first user ID: "; getline(cin, user1Id);
            cout << "Enter second user ID: "; getline(cin, user2Id);
            profile.showMutualFriends(user1Id, user2Id);
        } else if (choice == 9) {
            string userId, groupName;
            cout << "Enter your user ID: "; getline(cin, userId);
            cout << "Enter group name: "; getline(cin, groupName);
            profile.sendFriendRequestToGroup(userId, groupName);
        } else if (choice == 10) {
            string userId;
            cout << "Enter user ID: "; getline(cin, userId);
            profile.suggestFriends(userId);
        } else if (choice == 11) {
            profile.listAllUsers();
        } else if (choice == 12) {
            string userId;
            cout << "Enter user ID: "; getline(cin, userId);
            profile.listFriends(userId);
        } else if (choice == 13) {
            string userId;
            cout << "Enter user ID: "; getline(cin, userId);
            profile.listPendingRequests(userId);
        } else if (choice == 14) {
            string userId;
            cout << "Enter your user ID: "; getline(cin, userId);
            profile.editProfile(userId);
        } else if (choice == 15) {
            string userId, friendId;
            cout << "Enter your user ID: "; getline(cin, userId);
            cout << "Enter friend's ID to remove: "; getline(cin, friendId);
            profile.deleteFriend(userId, friendId);
        } else if (choice == 16) {
            string userId, senderId;
            cout << "Enter your user ID: "; getline(cin, userId);
            cout << "Enter ID of request to reject: "; getline(cin, senderId);
            profile.rejectFriendRequest(userId, senderId);
        } else if (choice == 17) {
            string senderId, groupName, message;
            cout << "Enter your user ID: "; getline(cin, senderId);
            cout << "Enter group name: "; getline(cin, groupName);
            cout << "Enter message: "; getline(cin, message);
            profile.sendGroupMessage(senderId, groupName, message);
        } else if (choice == 18) {
            string userId;
            cout << "Enter user ID to view: "; getline(cin, userId);
            profile.viewUserProfile(userId);
        } else if (choice == 0) {
            cout << "Exiting program.\n";
            break;
        } else {
            cout << "Invalid choice. Please try again.\n";
        }
    }
    return 0;
}



