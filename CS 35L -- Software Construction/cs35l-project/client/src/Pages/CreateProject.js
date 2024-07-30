import React, { useState } from 'react';
import './CreateProject.css';
import mainLogo from '../Files/bruinsource_logo.png'
import history from '../history';
import axios from 'axios';
import { useAuth } from '../Shared/ProvideAuth';
import { getCurrentDate } from '../Shared/CommonFunctions';

const CreateProject = (props) => {
    const [name, setName] = useState(null)
    const [description, setDescription] = useState(null)
    const [tags, setTags] = useState(null)
    const [github, setGithub] = useState(null)
    const [date_created, setDateCreated] = useState(null)
    const [last_updated, setLastUpdated] = useState(null)
    const [collaborators, setCollaborators] = useState([])
    const [requests, setRequests] = useState([])

    const [alert, setAlert] = useState(null)

    const auth = useAuth()

    let axiosConfig = {
        headers: {
            'Content-Type': 'application/json;charset=UTF-8',
            "Access-Control-Allow-Origin": "*",
        }
      };

    const onCreateProject = async () => {

        let userName = auth.username

        if (userName === null) {
            setAlert('Must be logged in to create a project.')
            setTimeout(() => history.push('/'), 3000)
            return
        }

        let id = null

        await axios.get(`/api/users/${userName}`)
            .then(res => {
                id = res.data.id
            })
            .catch(err => console.log(err))

        let formattedTags = tags.split(',');

        let projectData = {
            name: name,
            description: description,
            tags: formattedTags,
            github: github,
            date_created: getCurrentDate(),
            last_updated: getCurrentDate(),
            author: id,
            collaborators: collaborators,
            requests: requests,
        }

        axios.post('/api/projects/create', projectData, axiosConfig)
            .then(res => {
                console.log(res)
            })
            .catch(err => {
                console.error(err)
            })
        history.push('/dashboard')
    }

    return (
        <div className="CreateProject">
            <img src={mainLogo} className="MainLogo" alt="mainLogo" />
            <h2> Create A New Project </h2>
            <form className="Inputs">
                <div className="ProjectName">
                    <input
                        type="text"
                        placeholder="Project Name..."
                        required
                        onChange={(input) => setName(input.target.value)}
                    />
                </div>
                <div className="ProjectDescription">
                    <textarea
                        type="text"
                        placeholder="Project Description..."
                        required
                        onChange={(input) => setDescription(input.target.value)}
                    />
                </div>
                <div className="ProjectTags">
                    <input
                        type="text"
                        placeholder="Project Tags (comma-separated)..."
                        required
                        onChange={(input) => setTags(input.target.value)}
                    />
                </div>
                <div className="ProjectGitHub">
                    <input
                        type="text"
                        placeholder="http://github.com/..."
                        onChange={(input) => setGithub(input.target.value)}
                    />
                </div>
                <div>
                    <p id="error-text">
                        {alert}
                    </p>
                </div>
                <div className="Buttons">
                    <button type="button" className="BackToProjects" onClick={() => history.push('/dashboard')}>Back to Projects</button>
                    <button type="button" className="CreateNewProject" onClick={onCreateProject}>Create New Project</button>
                </div>
            </form>
        </div>
    );
}

export default CreateProject;