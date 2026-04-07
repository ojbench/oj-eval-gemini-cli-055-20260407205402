#include <iostream>
#include <algorithm>
#include <string>
#include "base.hpp"

using namespace std;

struct date {
    int year, month, day;
    date() = default;
    date(int y, int m, int d) : year(y), month(m), day(d) {}
    
    friend istream& operator>>(istream& is, date& d) {
        is >> d.year >> d.month >> d.day;
        return is;
    }
    
    bool operator<(const date& other) const {
        if (year != other.year) return year < other.year;
        if (month != other.month) return month < other.month;
        return day < other.day;
    }
    
    bool operator<=(const date& other) const {
        return !(other < *this);
    }
    
    bool operator==(const date& other) const {
        return year == other.year && month == other.month && day == other.day;
    }
    
    int to_days() const {
        return year * 360 + month * 30 + day;
    }
};

class mail : public object {
protected:
    string postmark;
    date send_date;
    date arrive_date;

public:
    mail() = default;

    mail(string _contain_, string _postmark_, date send_d, date arrive_d) : object(_contain_) {
        postmark = _postmark_;
        send_date = send_d;
        arrive_date = arrive_d;
    }

    virtual string send_status(int y, int m, int d) override {
        return "not send";
    }

    virtual string type() override {
        return "no type";
    }

    virtual void print() override {
        object::print();
        cout << "[mail] postmark: " << postmark << endl;
    }

    virtual void copy(object *o) override {
        if (this == o) return;
        contain = reinterpret_cast<mail *>(o)->contain;
        postmark = reinterpret_cast<mail *>(o)->postmark;
        send_date = reinterpret_cast<mail *>(o)->send_date;
        arrive_date = reinterpret_cast<mail *>(o)->arrive_date;
    }
    
    virtual ~mail() {}
};

class air_mail : public mail {
protected:
    string airlines;
    date take_off_date;
    date land_date;

public:
    air_mail() = default;

    air_mail(string _contain_, string _postmark_, date send_d, date arrive_d, date take_off, date land, string _airline) 
        : mail(_contain_, _postmark_, send_d, arrive_d) {
        take_off_date = take_off;
        land_date = land;
        airlines = _airline;
    }

    virtual string send_status(int y, int m, int d) override {
        date ask_date(y, m, d);
        if (ask_date < send_date)
            return "mail not send";
        else if (ask_date < take_off_date)
            return "wait in airport";
        else if (ask_date < land_date)
            return "in flight";
        else if (ask_date < arrive_date)
            return "already land";
        else
            return "already arrive";
    }

    virtual string type() override {
        return "air";
    }

    virtual void print() override {
        mail::print();
        cout << "[air] airlines: " << airlines << endl;
    }

    virtual void copy(object *o) override {
        if (this == o) return;
        mail::copy(o);
        take_off_date = reinterpret_cast<air_mail *>(o)->take_off_date;
        land_date = reinterpret_cast<air_mail *>(o)->land_date;
        airlines = reinterpret_cast<air_mail *>(o)->airlines;
    }
    
    virtual ~air_mail() {}
};

class train_mail : public mail {
protected:
    string *station_name;
    date *station_time;
    int len;

public:
    train_mail() : station_name(nullptr), station_time(nullptr), len(0) {}

    train_mail(string _contain_, string _postmark_, date send_d, date arrive_d, string *sname, date *stime, int station_num)
        : mail(_contain_, _postmark_, send_d, arrive_d) {
        len = station_num;
        if (len > 0) {
            station_name = new string[len];
            station_time = new date[len];
            for (int i = 0; i < len; ++i) {
                station_name[i] = sname[i];
                station_time[i] = stime[i];
            }
        } else {
            station_name = nullptr;
            station_time = nullptr;
        }
    }

    virtual string send_status(int y, int m, int d) override {
        date ask_date(y, m, d);
        if (ask_date < send_date)
            return "mail not send";
        if (!(ask_date < arrive_date))
            return "already arrive";
            
        if (len == 0) {
            return "wait in station";
        }
        
        if (ask_date < station_time[0])
            return "wait in station";
            
        for (int i = 0; i < len - 1; ++i) {
            if (!(ask_date < station_time[i]) && ask_date < station_time[i+1]) {
                return "run between " + station_name[i] + " and " + station_name[i+1];
            }
        }
        
        return "run between " + station_name[len - 1] + " and destination";
    }

    virtual string type() override {
        return "train";
    }

    virtual void print() override {
        mail::print();
        cout << "[train] station_num: " << len << endl;
    }

    virtual void copy(object *o) override {
        if (this == o) return;
        mail::copy(o);
        train_mail *other = reinterpret_cast<train_mail *>(o);
        
        string *new_station_name = nullptr;
        date *new_station_time = nullptr;
        
        if (other->len > 0) {
            new_station_name = new string[other->len];
            new_station_time = new date[other->len];
            for (int i = 0; i < other->len; ++i) {
                new_station_name[i] = other->station_name[i];
                new_station_time[i] = other->station_time[i];
            }
        }
        
        if (station_name) delete[] station_name;
        if (station_time) delete[] station_time;
        
        len = other->len;
        station_name = new_station_name;
        station_time = new_station_time;
    }
    
    virtual ~train_mail() {
        if (station_name) delete[] station_name;
        if (station_time) delete[] station_time;
    }
};

class car_mail : public mail {
protected:
    int total_mile;
    string driver;

public:
    car_mail() = default;

    car_mail(string _contain_, string _postmark_, date send_d, date arrive_d, int mile, string _driver)
        : mail(_contain_, _postmark_, send_d, arrive_d) {
        total_mile = mile;
        driver = _driver;
    }

    virtual string send_status(int y, int m, int d) override {
        date ask_date(y, m, d);
        if (ask_date < send_date)
            return "mail not send";
        else if (!(ask_date < arrive_date))
            return "already arrive";
        else {
            int used_time = ask_date.to_days() - send_date.to_days();
            int total_time = arrive_date.to_days() - send_date.to_days();
            double current_mile = (double)used_time / total_time * total_mile;
            return to_string(current_mile);
        }
    }

    virtual string type() override {
        return "car";
    }

    virtual void print() override {
        mail::print();
        cout << "[car] driver_name: " << driver << endl;
    }

    virtual void copy(object *o) override {
        if (this == o) return;
        mail::copy(o);
        total_mile = reinterpret_cast<car_mail *>(o)->total_mile;
        driver = reinterpret_cast<car_mail *>(o)->driver;
    }
    
    virtual ~car_mail() {}
};

void obj_swap(object *&lhs, object *&rhs) {
    object *temp = lhs;
    lhs = rhs;
    rhs = temp;
}