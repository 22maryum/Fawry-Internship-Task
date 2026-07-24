#include <iostream>
#include <vector>
#include <map>
#include <string>

using namespace std;

enum CarType { PRIVATE, TRUCK, BUS };

string carTypeName(CarType type) {
    if (type == PRIVATE) return "Private";
    if (type == TRUCK) return "Truck";
    return "Bus";
}

// Info. from the physical radar
struct Observation {
    string plateNumber;
    CarType carType;
    int speed;
    bool seatbeltFastened;
};

class Rule {
public:
    virtual bool isViolated(Observation obs) = 0;
    virtual string message(Observation obs) = 0;
    virtual double fee() = 0;
    virtual string name() = 0;
};

class SpeedRule : public Rule {
    CarType carType;
    int maxSpeed;
    double feeAmount;
public:
    SpeedRule(CarType carType, int maxSpeed, double feeAmount) {
        this->carType = carType;
        this->maxSpeed = maxSpeed;
        this->feeAmount = feeAmount;
    }
    bool isViolated(Observation obs) {
        return obs.carType == carType && obs.speed > maxSpeed;
    }
    string message(Observation obs) {
        return "speed of " + to_string(obs.speed) + " exceeded max allowed " + to_string(maxSpeed);
    }
    double fee() { return feeAmount; }
    string name() { return carTypeName(carType) + " speed limit"; }
};

class SeatbeltRule : public Rule {
    double feeAmount;
public:
    SeatbeltRule(double feeAmount) {
        this->feeAmount = feeAmount;
    }
    bool isViolated(Observation obs) {
        return !obs.seatbeltFastened;
    }
    string message(Observation obs) {
        return "Seatbelt not fastned ";
    }
    double fee() { return feeAmount; }
    string name() { return "Seatbelt rule"; }
};

struct Violation {
    string description;
    double fee;
};

// The violations for one car
class Fine {
public:
    string plateNumber;
    vector<Violation> violations;

    double totalAmount() {
        double total = 0;
        for (int i = 0; i < violations.size(); i++) {
            total += violations[i].fee;
        }
        return total;
    }

    void print() {
        cout << "Traffic fine for car " << plateNumber << endl;
        cout << "Total amount: " << (int)totalAmount() << " EGP" << endl;
        cout << "Violations:" << endl;
        for (int i = 0; i < violations.size(); i++) {
            cout << "- " << violations[i].description << " : " << (int)violations[i].fee << " EGP" << endl;
        }
    }
};

class Radar {
    vector<Rule*> rules;
    vector<Fine> fines;
    map<string, int> violatedRuleCounts;

public:
    void addRule(Rule* rule) {
        rules.push_back(rule);
    }

    void observe(Observation obs) {
        vector<Violation> violations;
        for (int i = 0; i < rules.size(); i++) {
            if (rules[i]->isViolated(obs)) {
                Violation v;
                v.description = rules[i]->message(obs);
                v.fee = rules[i]->fee();
                violations.push_back(v);
                violatedRuleCounts[rules[i]->name()]++;
            }
        }
        if (violations.size() > 0) {
            Fine fine;
            fine.plateNumber = obs.plateNumber;
            fine.violations = violations;
            fines.push_back(fine);
        }
    }

    vector<Fine> getFines() {
        return fines;
    }

    void printAllFines() {
        for (int i = 0; i < fines.size(); i++) {
            cout << fines[i].plateNumber << " : " << (int)fines[i].totalAmount() << " EGP" << endl;
        }
    }

    void printAllViolatedRules() {
        for (map<string, int>::iterator it = violatedRuleCounts.begin(); it != violatedRuleCounts.end(); it++) {
            cout << it->first << " : " << it->second << endl;
        }
    }
};

int main() {
    Radar radar;
    radar.addRule(new SeatbeltRule(100));
    radar.addRule(new SpeedRule(TRUCK, 60, 300));
    radar.addRule(new SpeedRule(PRIVATE, 80, 300));

    Observation obs;
    obs.plateNumber = "ABC1234";
    obs.carType = PRIVATE;
    obs.speed = 94;
    obs.seatbeltFastened = false;

    radar.observe(obs);

    vector<Fine> fines = radar.getFines();
    for (int i = 0; i < fines.size(); i++) {
        fines[i].print();
    }

    cout << endl << "All fines:" << endl;
    radar.printAllFines();

    cout << endl << "Violated rules:" << endl;
    radar.printAllViolatedRules();

    return 0;
}