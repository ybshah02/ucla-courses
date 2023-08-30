import React from 'react';
import './TaskDashBoard.css';
import mainLogo from '../Files/bruinsource_logo.png'
import searchIcon from '../Files/search_icon.png'
import history from '../history';

//const {client} = require('./db.js')

class TaskDashBoard extends React.Component {

    constructor(props) {
        super(props);
        this.state = {
          tasks: []
        }
    }
    /*Have to find out how to get project id for this particular page*/
    TaskList() {
        return fetch('https://localhost:8000/api/projects/:projectId/tasks')
        .then(res =>this.setState({projects: res.rows}));
    }
    renderTableData() {
        return this.state.tasks.map((task, index) => {
            const {id, name, description, tags, project_id, assigned_user, date_created, date_due, is_finished, date_finished} = task
            return (
                <tr key={name}>
                    <td>{name}</td>
                    <td>{assigned_user}</td>
                    <td>{date_created}</td>
                    <td>{is_finished}</td>
                </tr>
            )
        })
    }
    render() {
        return (
            <div className="TaskDashBoard">
                <img src={mainLogo} className="MainLogo" alt="mainLogo"/>
                <h2> Tasks </h2>
                <form>
                    <input
                    type="text"
                    placeholder="Search for a task..."
                    />
                </form>
                <button type="button" className="Search"> 
                    <img src={searchIcon} width="50px" alt="searchIcon" ></img>
                </button>
                <button type="button" className="Create" onClick={() => history.push('/createtask')}>Create a Task</button>
                <button type="button" className="BackToProjects" onClick={() => history.push('/dashboard')}>Back to My Projects</button>
                <div className="TaskList">
                    <table className="TaskListTable">
                        <thead className="TaskListTableHead">
                            <tr>
                                <td>TASK</td>
                                <td>CONTRIBUTOR</td>
                                <td>DATE CREATED</td>
                                <td>STATUS</td>
                            </tr>
                        </thead>
                        <tbody>
                            {this.renderTableData()}
                        </tbody>
                    </table>
                </div>
            </div>
        );
    }
}

export default TaskDashBoard;