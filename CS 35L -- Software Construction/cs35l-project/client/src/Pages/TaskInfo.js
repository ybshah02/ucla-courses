import React from 'react';
import './TaskInfo.css';
import mainLogo from '../Files/bruinsource_logo.png'
import history from '../history.js'
class TaskInfo extends React.Component {
    render() {
        return (
            <div className="TaskInfo">
                <img src={mainLogo} className="MainLogo" alt="mainLogo"/>
                <h1>Task Name</h1>
                <p>Lorem ipsum dolor amet</p>
                    <div className="Buttons">
                        <button type="button" className="BackToTasks" onClick={() => history.push('')}>Back to Tasks</button>
                        <button type="button" className="SubmitTask">Submit Task</button>
                    </div>
            </div>
        );
    }
}

export default TaskInfo;