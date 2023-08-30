//main app

/** setup server **/
const express = require('express');
const session = require('express-session');
const helmet = require('helmet');
const http = require('http');
const https = require('https');
const fs = require('fs');
const path = require('path');

/** setup database  **/
const {connectdb} = require('./db.js')
connectdb()

/** middleware **/
const app = express();
app.use(helmet());
app.use(express.json());
app.use(express.urlencoded({extended: false}));

const PORT = process.env.PORT || 8000;

/** main connection pool **/

const user = require('./user.js')
const project = require('./project.js')
// const task = require('./task.js')

/**  routes **/

////////  USER APIs    ////////


/** Register user **/
app.post('/api/register', user.registerUser)

/** Retrieve user by login **/
app.post('/api/login', user.validateLogin)

/** Retrieve all users query **/
app.get('/api/users', user.getUsers);

/** Retrieve user by username **/
app.get('/api/users/:username', user.getUserByUsername);

/* ID TO USERNAME */
app.get('/api/users/idtouser/:id', user.getUserByID);

////////  PROJECT APIs    ////////

/** Create a project **/
app.post('/api/projects/create', project.createProject);

/** Delete a project **/
app.get('/api/projects/:projectId/delete', project.deleteProject);

/** Retrieve all available projects **/
app.get('/api/projects', project.getProjects);

/** Retrieve a project by project id **/
app.get('/api/projects/projectidpath/:projectId', project.getProjectById);

/** Retrieve projects owned by a user **/
app.get('/api/projects/user/:username', project.getProjectsByUser);

/** Search through all available projects **/
app.post('/api/projects/tags', project.searchProjectsByTags);

/** Retrieve collaboration requests to user for projects owned **/
app.get('/api/projects/requests', project.getAllRequests);

/** Retrieve collaboration requests to user for projects owned **/
app.get('/api/projects/requests/:projectId', project.getProjectRequests);

/** Create a request for a project **/
app.post('/api/projects/requests/create', project.createRequest);

/** Join a team for a project **/
app.post('/api/projects/requests/join', project.joinTeam);

/** Remove a user from a team **/
app.post('/api/projects/requests/remove', project.deleteRequest);

/** Search for a project **/
app.get('/api/projects/searchproject/:search', project.searchProjects)

////////  TASK APIs    ////////

/** Create a task for a project **/
// app.post('/api/projects/tasks/create', task.createTask);

/** Delete a task **/
// app.get('/api/projects/tasks/delete/:taskId', task.deleteTask);

/** Retrieve all tasks for a project **/
// app.get('/api/projects/tasks/:projectId', task.getTasks);

app.listen(PORT, function(err) {
  if (err) console.log(err);
  console.log(`Server started on port ${PORT}`);
});