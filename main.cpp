#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <math.h>
#include <bitset>
#include <chrono>
#include <ctime>
using namespace std::chrono;
//Ruben Holubek s1006591 & Gunnar Noordbruis s1008953
using namespace std;

struct student{
    // this students left half of answer
    long int answerA=0;
    // This students right half of answer
    long int answerB=0;
    // the score of this student
    int score = 0;
};

struct ansModel{
    // the ans model of this ans model
    long int ans=0;
    // the number of ans models with the this score
    long int number=1;
    // the score of all students stored in 5 bits each
    uint64_t sInt=0;
};

//n=students,a+b=questionscompAnsModel
int n,a,b;
//same=number of ans Models yield same score
int same=0;
//left half of questions
vector<ansModel> ansLeft;
//right half of questions
vector<ansModel> ansRight;
//total target score
uint64_t target;
//students
vector<student> Students;
//left and right side of solution left should be first and if its not it's swapped
vector<ansModel> solutions;
//number of solutions so that we do not have to track each solution once more exists than 1.
unsigned long long int numberSolutions=0;


/*
 * print ansModels and student to standard output
 */
void printAnsModel(ansModel& s){
    if(s.number>1){
        cout << "number: \t" << s.number << ", " << s.sInt << endl;
    }
    else
        cout << "model: \t" << s.ans << ", " << s.sInt << endl;
}
void printAnsModel(vector<ansModel>& vec, int to){
    int number;
    if(to==0)
        number=vec.size();
    else
        number = to;
    for(int i=0; i<number; i++){
        printAnsModel(vec.at(i));
    }
    cout << "\n";
}
void printStudents(){
    for(auto &s : Students)
        cout << "student: "<< s.answerA << " " <<  s.answerB << " " << s.score << endl;
}

/*
 * compares two students
 */
bool compStudents(student& x, student& y){
    int as = x.score;
    int bs = y.score;
    if(as>a)
        as=abs(as-(a+b));
    if(bs>a)
        bs=abs(bs-(a+b));
    return as<bs;
}

/*
 * compares two ansModels
 */
long long int compAnsModel(ansModel& a, ansModel& b){
    return (a.sInt-b.sInt);
}

/*
 * converts compAnsModel to a boolean
 */
bool boolCompAnsModel(ansModel& a, ansModel& b){
    return compAnsModel(a,b)<0;
}

/*
 * When two answer models have the same score we can compress these.
 */
vector<ansModel> compress(vector<ansModel>& a){
    vector<ansModel> result;
    for(unsigned long long int i = 0; i<a.size()-1;i++){
        if(compAnsModel(a[i],a[i+1])==0)
            a[i+1].number+=a[i].number;
        else
            result.push_back(a[i]);
    }
    result.push_back(a[a.size()-1]);
    return result;
}

/*
 * combines left and right answer into one answer
 */
string combineHalfs(unsigned long int fst, unsigned long int snd){
    unsigned long long int temp = fst;
    temp = temp<<b;
    temp+=snd;
    string String = bitset< 64 >( temp ).to_string();
    return String.substr((String.size()-(a+b)),String.size());
}

/*
 * flipts given answers if correct > (m+1)/2
 */
string flip(string str){
    string result;
    for(char i : str) {
        if(i == '1')
            result+= '0';
        else
            result+= '1';
    }
    return result;
}

/*
 * returns number of correct answers in
 */
int  XOR(long int  ans1, long int  ans2, int size){
    return (size-__builtin_popcountl(ans1^ans2));
}

/*
 * corrects Target score to the sorted student vector
 */
void correctTarget(){
    unsigned long long int tempBin=0;
    for(auto &s : Students){
        tempBin<<=5;
        tempBin+=s.score;
    }
    target = tempBin;
}

/*
 * Performs binSearch on vector ans to find object x in lg(n) time.
 */
long long int binSearch(vector<ansModel>& ans, int low, int high, unsigned long long int x){
    if (high >= low) {
        int middle = low + (high - low) / 2;

        if (ans[middle].sInt == x)
            return middle;

        //Smaller go left
        if (ans[middle].sInt > x)
            return binSearch(ans, low, middle - 1, x);

        // Larger go right
        return binSearch(ans, middle + 1, high, x);
    }
    // object not in list
    return -1;
}

/*
 * returns scores for all students using ansModel as answer model
 */
ansModel answerCheck(bool ans, unsigned long int model, int size){
    long int tempAns;
    ansModel result;
    unsigned long long int tempBin=0;
    for(int j = 0; j < n;j++){
        if(ans)
            tempAns = Students[j].answerA;
        else
            tempAns = Students[j].answerB;
        int numCorrect = XOR(model, tempAns, size);
        if(numCorrect>Students[j].score) {
            result.ans = -1;
            return result;
        }
        //tempBin+=numCorrect*(pow(32,(n-j-1))); does the same but harder to understand.
        tempBin<<=5;
        tempBin+=numCorrect;
    }
    result.sInt = tempBin;
    result.ans = model;
    return result;
}

/*
 * Checks all answer models and thus creates list of possible models.
 */
vector<ansModel> answerCreation(bool ans, int size){
    vector<ansModel> result;
    if(size==0 && ans){
        ansModel ansFake;
        result.push_back(ansFake);
    }
    else{
        for(unsigned long int i = 0 ; i < pow(2,size);i++){
            ansModel hulpAns = answerCheck(ans, i, size);
            if(hulpAns.ans!=-1){
                result.push_back(hulpAns);
            }
        }
    }
    return result;
}

/*
 * Creates all linear combinations that result in a.score + b.score == target
 * thus those answer models who are correct to the problem
 */
void linear_combinations(vector<ansModel>& a, vector<ansModel>& b, bool swap){
    long long int i;
    for(auto &ans : b){
        i = binSearch(a, 0, a.size()-1, (target-ans.sInt) );
        if(i!=-1){
            if(numberSolutions==0){
                solutions.push_back(a.at(i));
                solutions.push_back(ans);
            }
            numberSolutions += a.at(i).number;
        }
    }
    if(swap && solutions.size()>=2){
        std::swap(solutions[0],solutions[1]);
    }
}

/*
 * Ouputs final conclusion
 */
void finish(){
    if (numberSolutions > 1 || numberSolutions==0)
        cout << numberSolutions << " solutions";
    else
        cout << combineHalfs(solutions[0].ans,solutions[1].ans);
}

/*
 * reads input data, performs conversion of input if this provides benefits
 * Sorts student vector and fixes the target score at the end.
 */
void readData(){
    int m;
    cin >> n >> m;
    a=m/2;
    b=m-a;
    string str;
    student st;
    int readInt;
    ansLeft.resize(pow(2,a));
    ansRight.resize(pow(2,b));
    for(int s = 0; s<n; s++){
        vector<bool> help;
        cin >> str >> readInt;
        if(readInt>((m+1)/2)){
            str = flip(str);
            readInt = m-readInt;
        }
        st.score = readInt;
        if(a!=0)
            st.answerA = stol(str.substr(0,a), nullptr, 2);
        st.answerB = stol(str.substr(a,b), nullptr, 2);
        //cout << readInt << " " << m << endl;
        Students.push_back(st);
    }
    sort(Students.begin(),Students.end(),compStudents);
    correctTarget();
}


int main(){
    readData();
    high_resolution_clock::time_point start = high_resolution_clock::now();
    ansLeft = answerCreation(true, a);
    ansRight = answerCreation(false, b);
    // if either the left or right side have no possible answers you will never be able to create linear combinations
    // thus we can then skip this step
    if(ansRight.size()!=0 && ansLeft.size()!=0){
        // it is better to binSearch in the smallest vector thus we check this
        if(ansRight.size()<ansLeft.size()){
            sort(ansRight.begin(),ansRight.end(),boolCompAnsModel);
            ansRight = compress(ansRight);
            linear_combinations(ansRight, ansLeft, true);
        }
        else{
            sort(ansLeft.begin(),ansLeft.end(),boolCompAnsModel);
            ansLeft = compress(ansLeft);
            linear_combinations(ansLeft, ansRight, false);
        }
    }
    finish();
    high_resolution_clock::time_point end = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>( end - start ).count();
    cout << "\nElapsed time: " << duration << "ms" << endl;
    return 0;
}