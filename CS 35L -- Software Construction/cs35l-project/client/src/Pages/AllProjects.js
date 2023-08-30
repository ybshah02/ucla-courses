import React, { useEffect, useState } from 'react';
import './AllProjects.css';
import mainLogo from '../Files/bruinsource_logo.png'
import searchIcon from '../Files/search_icon.png'
/*import { getProjectById } from '../../server/project';*/
import history from '../history';
import axios from 'axios';
import { Bars } from 'react-loading-icons'
import ProjectTable from '../Components/ProjectTable';


const AllProjects = (props) => {

    const [projects, setProjects] = useState(null);
    const [search, setSearch] = useState(null);

    const [dataLoaded, setDataLoaded] = useState(false)

    const submitSearch = () => {

        if (search === "") {
            return getDefaultProjects();
        }

        axios.get(`api/projects/searchproject/${search}`)
            .then(res => {
                let data = res.data.map(element => {
                    let copy = element
                    if (!copy.collaborators) {
                        copy.collaborators = 0
                    } else {
                        copy.collaborators = copy.collaborators.length
                    }
                    return copy
                });
                setProjects(data)
            });
    }

    const getDefaultProjects = () => {
        axios.get('/api/projects')
            .then(res => {
                let data = res.data.map(element => {
                    let copy = element
                    if (!copy.collaborators) {
                        copy.collaborators = 0
                    } else {
                        copy.collaborators = copy.collaborators.length
                    }
                    return copy
                });
                setProjects(data)
            });
    }

    useEffect(() => {
        if (projects) {
            setDataLoaded(true)
            if (projects.length == 0) {
                // make some text to show that none exist for this search term
            }
        }
    }, [projects])

    useEffect(() => {
        getDefaultProjects()
    }, [])

    const renderTableData = () => {
        console.log(projects)
        if (!projects || projects.length === 0) {
            return <tr> No entries exist for this search. </tr>
        } else {
            return projects.map((project, index) => {
                const { id, name, description, tags, date_created, last_updated, author, collaborators, requests } = project

                var d = new Date(date_created)
                d = d.toDateString()

                var collaboratorsExist = false
                if (collaborators && collaborators.length) {
                    if (collaborators.length > 0) {
                        collaboratorsExist = true
                    }
                }

                return (
                    <tr key={index}>
                        <td>{name}</td>
                        <td>{author}</td>
                        <td>{d}</td>
                        <td>{tags}</td>
                        <td>{collaboratorsExist ? collaborators : 'No collaborators'}</td>
                    </tr>
                )
            })
        }
    }

    return (
        <div className="AllProjects">
            <img src={mainLogo} className="MainLogo" alt="mainLogo" />
            <h2> All Projects </h2>
            <form>
                <input
                    type="text"
                    placeholder="Search from all projects..."
                    onChange={(input) => setSearch(input.target.value)}
                />
            </form>
            <button type="button" className="Search" onClick={submitSearch}>
                <img src={searchIcon} width="50px" alt="searchIcon" ></img>
            </button>
            <button type="button" className="Create" onClick={() => history.push('/createproject')}>Create New Project</button>
            <button type="button" className="BackToProjects" onClick={() => history.push('/dashboard')}>Back to My Projects</button>
            <button type="button" className="Interest" onClick={() => history.push('/searchinterest')}>Search By Interest</button>
            <div className="ProjectList">
                {!dataLoaded ?
                    <div className="LoadingDiv"> <Bars fill="#005587" /> </div>
                    :
                    <ProjectTable data={projects} ></ProjectTable>
                }
            </div>
        </div>
    );
}

export default AllProjects;