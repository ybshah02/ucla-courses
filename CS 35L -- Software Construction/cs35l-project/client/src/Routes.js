import React, { Component } from "react";
import { Router, Switch, Route, Link } from "react-router-dom";

import Login from './Pages/Login.js';
import Dashboard from './Pages/Dashboard.js'
import CreateProject from './Pages/CreateProject.js'
import Register from './Pages/Register.js'
import TaskInfo from './Pages/TaskInfo.js'
import ProjectInfo from './Pages/ProjectInfo'
import TaskDashboard from './Pages/TaskDashBoard.js'
import AllProjects from './Pages/AllProjects.js'
import history from './history';
import { ProvideAuth } from "./Shared/ProvideAuth.js";
import Header from "./Components/Header.js";
import SearchByInterest from './Pages/SearchByInterest.js';

export default class Routes extends Component {
    render() {
        return (
            <ProvideAuth>
                <Router history={history}>
                    <div className="app">
                        <Header />
                        <Switch>
                            <Route exact path="/" component={Login} />
                            <Route exact path="/register" component={Register} />
                            <Route exact path="/dashboard" component={Dashboard} />
                            <Route exact path="/createproject" component={CreateProject} />
                            <Route exact path="/taskinfo" component={TaskInfo} />
                            <Route exact path="/projectinfo" component={ProjectInfo} />
                            <Route exact path="/taskdashboard" component={TaskDashboard} />
                            <Route exact path="/allprojects" component={AllProjects} />
                            <Route exact path="/searchinterest" component={SearchByInterest} />
                        </Switch>
                    </div>
                </Router>
            </ProvideAuth>
        )
    }
}
