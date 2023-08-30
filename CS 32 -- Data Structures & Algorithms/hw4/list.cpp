
void listAll(string path, const MenuItem* m) // two-parameter overload
 {
     if (m->menuItems() != nullptr){
         for(vector<MenuItem*>::const_iterator itr = m->menuItems()->begin(); itr != m->menuItems()->end(); itr++){
             string fileName = (*itr)->name();
             string menu = path + fileName;
             cout << menu << endl;
             listAll(menu + "/", *itr);
         }
     }
 }

