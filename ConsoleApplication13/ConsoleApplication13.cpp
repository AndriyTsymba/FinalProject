#include <iostream>
#include <string>
#include <vector>
#include <fstream>
using namespace std;

namespace RPGGame {
    class RPGException {
    public:
        explicit RPGException(const string& message) : message_(message) {}
       string what() const { return message_; }

    private:
        string message_;
    };
    class InvalidActionException : public RPGException {
    public:
        explicit InvalidActionException(const string& message)
            : RPGException("Invalid Action: " + message) {
        }
    };
    class Character {
    protected:
        string name;
        int health;

    public:
        Character(const string& name, int health) : name(name), health(health) {}
        virtual ~Character() {}

        virtual void attack(Character& target) = 0;

        bool isAlive() const { return health > 0; }
        const string& getName() const { return name; }

        void takeDamage(int damage) {
            health -= damage;
            if (health < 0) health = 0;
        }

        void displayStatus() const {
            cout << name << " (Health: " << health << ")\n";
        }
    };

    class Player : public Character {
    private:
        int level;
        int experience;

    public:
        Player(const string& name)
            : Character(name, 100), level(1), experience(0) {
        }
        void attack(Character& target) override {
            cout << name << " attacks " << target.getName() << "!\n";
            target.takeDamage(20);
        }
        void gainExperience(int amount) {
            experience += amount;
            if (experience >= 100) {
                levelUp();
            }
        }
        void levelUp() {
            level++;
            experience = 0;
            health = 100;
            cout << name << " leveled up to level " << level << "!\n";
        }
        void saveToFile(const string& filename) {
            ofstream outFile(filename);
            if (outFile.is_open()) {
                outFile << name << "\n";
                outFile << health << "\n";
                outFile << level << "\n";
                outFile << experience << "\n";
                outFile.close();
                cout << "Game saved successfully.\n";
            }
            else {
                throw RPGException("Unable to save the game.");
            }
        }
        void loadFromFile(const string& filename) {
            std::ifstream inFile(filename);
            if (inFile.is_open()) {
                std::getline(inFile, name);
                inFile >> health >> level >> experience;
                inFile.close();
                cout << "Game loaded successfully.\n";
            }
            else {
                throw RPGException("Unable to load the game.");
            }
        }
    };
    class Enemy : public Character {
    public:
        Enemy(const std::string& name, int health) : Character(name, health) {}

        void attack(Character& target) override {
            cout << name << " attacks " << target.getName() << "!\n";
            target.takeDamage(15);
        }
    };
    class GameManager {
    public:
        void startGame() {
            string playerName;
            cout << "Enter your character's name: ";
            cin >> playerName;

            Player player(playerName);
            Enemy enemy("Goblin", 70);

            cout << "The game begins!\n";
            player.displayStatus();
            enemy.displayStatus();

            while (player.isAlive() && enemy.isAlive()) {
                cout << "Choose an action: (1) Attack, (2) Save, (3) Load: ";
                int choice;
                cin >> choice;

                try {
                    switch (choice) {
                    case 1:
                        player.attack(enemy);
                        if (enemy.isAlive()) {
                            enemy.attack(player);
                        }
                        else {
                            cout << enemy.getName() << " is defeated!\n";
                            player.gainExperience(50);
                        }
                        break;
                    case 2:
                        player.saveToFile("savegame.txt");
                        break;
                    case 3:
                        player.loadFromFile("savegame.txt");
                        break;
                    default:
                        throw InvalidActionException("Unknown action selected.");
                    }
                }
                catch (const RPGException& e) {
                    cout << "Error: " << e.what() << "\n";
                }

                player.displayStatus();
                enemy.displayStatus();
            }

            if (player.isAlive()) {
                cout << "You have won the battle!\n";
            }
            else {
                cout << "Game over.\n";
            }
        }
    };
} 

int main() {
    RPGGame::GameManager gameManager;
    gameManager.startGame();
    return 0;
}