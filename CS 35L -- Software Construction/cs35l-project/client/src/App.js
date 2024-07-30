import logo from './logo.svg';
import React, {useEffect} from 'react';
import { BrowserRouter, Switch, Route } from "react-router-dom";
import './App.css';
import axios from 'axios';
import Routes from "./Routes.js"
import history from './history';
import {deleteUser, insertUser} from './Shared/backend-calls'

/*to-do
install react router and make header component + different pages
*/
 
  /* FORMAT TO INSERT A USER
  const values = {
    id: 1134,
    status: false,
    usename: 'myuser',
    password: 'testpass',
    email: 'howard',
    github: 'no',
    year_exp: 1,
    known_languages: ['javascript', 'python'],
    projects_worked: [1,3,]
  } 
  */

function App() {
  useEffect(() => {
  }, [])

  return (
    <div className="App">
      <Routes />
    </div>
  );
}

export default App;