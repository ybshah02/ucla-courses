
void removeOdds(vector<int>& v)
{
    int i = 0;
    while (i < v.size()){
        if (v[i] % 2 != 0){
            v.erase(v.begin() + i);
        }else{
            i++;
        }
    }
}

