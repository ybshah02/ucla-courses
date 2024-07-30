class Medium {
public:
    Medium(string senderId): m_senderId(senderId){}
    virtual ~Medium(){}
    virtual string connect() const = 0; // pure virtual
    string id() const{ // non-virtual
        return m_senderId;
    }
    virtual string transmit(string msg) const{ // non-pure virtual
        return "text: " + msg;
    }
private:
    string m_senderId;
};

class TwitterAccount: public Medium {
public:
    TwitterAccount(string senderId): Medium(senderId){}
    virtual ~TwitterAccount(){
        cout << "Destroying the Twitter account " + Medium::id() + "." << endl;
    }
    virtual string connect() const{
        return "Tweet";
    }
};

class Phone: public Medium {
public:
    Phone(string senderId, CallType callType): Medium(senderId), m_callType(callType){}
    virtual ~Phone(){
        cout << "Destroying the phone " + Medium::id() + "." << endl;
    }
    virtual string connect() const{
        return "Call";
    }
    virtual string transmit(string msg) const {
        if (m_callType == TEXT){
            return "text: " + msg;
        }else
            return "voice: " + msg;
    }
private:
    CallType m_callType;
};

class EmailAccount: public Medium {
public:
    EmailAccount(string senderId): Medium(senderId){}
    virtual ~EmailAccount(){
        cout << "Destroying the email account " + Medium::id() + "." << endl;
    }
    virtual string connect() const{
        return "Email";
    }
};
