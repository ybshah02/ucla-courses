
void removeBad(list<Movie*>& li)
{
    list<Movie*>::iterator itr = li.begin();
    while (itr != li.end()){
        Movie* mp = *itr;
        if (mp->rating() < 55){
            itr = li.erase(itr);
            delete mp;
        }else{
            itr++;
        }
    }
}

