import React, { useEffect, useState } from 'react';
import './ProjectInfo.css';
import mainLogo from '../Files/bruinsource_logo.png'
import history from '../history.js'
import axios from 'axios';
import { red } from '@mui/material/colors';
import { Typography } from '@mui/material';
import { makeStyles } from '@mui/styles';
import { Bars } from 'react-loading-icons';
import DeleteIcon from '@mui/icons-material/Delete';
import MyList from '../Components/List';
import { useAuth } from '../Shared/ProvideAuth';
import { getCurrentDate } from '../Shared/CommonFunctions';


const useStyles2 = makeStyles({
    custom: {
        color: "black",
        fontFamily: "Georgia"
    }
});

const ProjectInfo = (props) => {
    const [projectInfo, setProjectInfo] = useState(null)
    const [shouldButtonDisplay, setShouldButtonDisplay] = useState(true)
    if (!history) {
        history.push('/');
    }
    if (!history.location.state && !history.location.state[0]) {
        history.push('/dashboard')
    }

    const auth = useAuth()

    if (!auth.username) {
        history.push('/')
    }

    const onJoinTeam = async () => {

        let userName = auth.username

        if (userName === null) {
            setTimeout((() => history.push('/'), 3000))
            return
        }

        let id = null
        await axios.get(`api/users/${userName}`)
            .then(res => {
                id = res.data.id
            })
            .catch(err => console.log(err))


        let requesterData =
        {
            userId: id,
            projectId: historyProject,
            date_created: getCurrentDate(),
        }

        if (typeof (requesterData.userId) === "string") {
            requesterData.userId = parseInt(requesterData.userId)
        }

        let newCollaborator = true;
        await axios.get(`/api/projects/projectidpath/${requesterData.projectId}`)
            .then(res => {
                console.log(res)
                let existingCollaborators = res.data.collaborators;
                existingCollaborators.forEach(element => {
                    console.log(element)
                    if (typeof (element === "string")) {
                        element = parseInt(element)
                    }
                    if (requesterData.userId === element) {
                        //this means the user is already on the collaborator list
                        console.log(newCollaborator);
                        newCollaborator = false;
                    }
                })
            }
            )
            .catch(err => { console.log(err) })

        console.log(requesterData)
        if (newCollaborator) {
            axios.post('/api/projects/requests/join', requesterData)
                .then(res => {
                    console.log(res)
                    setTimeout((() => history.push('/dashboard'), 3000))
                    return
                })
                .catch(err => {
                    console.error(err)
                })
        }
        else {
            // alert the user?
        }
    }

    let historyProject = history.location.state[0];

    useEffect(() => {
        let userid = null
        axios.get(`/api/users/${auth.username}`)
            .then(res => {
                console.log(res)
                if (res.data && res.data != '') {
                    userid = res.data.id
                    axios.get(`/api/projects/projectidpath/${history.location.state[0]}`)
                        .then(res2 => {
                            console.log(res2)
                            var d = new Date(res2.data.date_created)
                            d = d.toDateString()
                            if (res2.data.collaborators && res2.data.collaborators.length > 0) {
                                let usernames = []
                                let length11 = res2.data.collaborators.length
                                res2.data.collaborators.forEach((element, index) => {
                                    axios.get(`/api/users/idtouser/${element}`)
                                        .then(res3 => {
                                            console.log(res3)
                                            usernames.push(res3.data.username)
                                            if (index === (length11 - 1)) {
                                                console.log(userid)
                                                console.log(usernames)
                                                let myObject = {
                                                    name: res2.data.name,
                                                    date_created: d,
                                                    description: res2.data.description,
                                                    github: res2.data.github,
                                                    requests: usernames,
                                                    isOwner: false,
                                                }
                                                if (userid == res2.data.author) {
                                                    let copy = myObject
                                                    copy.isOwner = true
                                                    myObject = copy
                                                    setShouldButtonDisplay(false)
                                                }
                                                console.log(myObject)
                                                setProjectInfo(myObject)
                                            }
                                        })
                                });
                            } else {
                                let myObject = {
                                    name: res2.data.name,
                                    date_created: d,
                                    description: res2.data.description,
                                    github: res2.data.github,
                                    requests: null,
                                    isOwner: false,
                                }
                                if (userid == res2.data.author) {
                                    let copy = myObject
                                    copy.isOwner = true
                                    myObject = copy
                                    setShouldButtonDisplay(false)
                                }
                                console.log(myObject)
                                setProjectInfo(myObject)
                            }
                        })
                        .catch(err => console.error(err))
                } else {
                    history.push('/')
                }
            })
    }, [])

    const allProjectInfo = () => {
        return (
            <React.Fragment>
                <div className="MainInfoGrid">
                    <div className="ProjectText">
                        <Typography variant="h3" gutterBottom component="div" sx={{ fontFamily: 'Georgia', fontWeight: 'bold' }}>
                            {projectInfo.name}
                        </Typography>
                        <Typography variant="h6" gutterBottom component="div" sx={{ fontFamily: 'Georgia' }}>
                            Date Created: {projectInfo.date_created}
                        </Typography>
                        <Typography variant="body1" gutterBottom component="div" sx={{ fontFamily: 'Georgia' }}>
                            {projectInfo.description}
                        </Typography>
                        <Typography variant="h6" gutterBottom component="div" sx={{ fontFamily: 'Georgia' }}>
                            <a href={`${projectInfo.github}`}>Project Github</a>
                        </Typography>
                    </div>
                    <div>
                        <MyList data={projectInfo.requests} isOwner={projectInfo.isOwner}></MyList>
                    </div>
                </div>
                <div className="Buttons">
                    <button type="button" className="BackToProjects" onClick={() => history.push('/dashboard')}>Back to Dashboard</button>
                    {shouldButtonDisplay &&
                        <button type="button" className="RequestAccess" onClick={onJoinTeam}>Join Team</button>
                    }
                </div>
            </React.Fragment>
        )
    }

    return (
        <div className="ProjectInfo">
            <img src={mainLogo} className="MainLogo" alt="mainLogo" />
            {projectInfo ?
                allProjectInfo()
                :
                <div className="LoadingDiv"> <Bars fill="#005587" /> </div>
            }
        </div>
    );
}

export default ProjectInfo;