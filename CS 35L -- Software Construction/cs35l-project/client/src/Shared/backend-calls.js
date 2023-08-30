/* file to store all example calls while I get backend working */

import axios from "axios";

export const insertUser = (userValues) => {
    /* can use userValues to change custom values in format below */
    const values = {
        status: false,
        username: 'myuser',
        password: 'testpass',
        email: 'howard',
        github: 'no',
        year_exp: 1,
        known_languages: ['javascript', 'python'],
        projects_worked: [1, 3]
    }

    axios.post('/insertuser', values)
        .then(res => console.log(res))
        .catch(err => console.log(err)) // add error handling here
}

export const deleteUser = (username) => {
    axios.post('/deleteuser', { username: username })
        .then(res => console.log(res))
        .catch(err => console.log(err))
}

export const getUserData = (username) => {
    let data = null;
    axios.post('/getuserinfo', { username: username })
        .then(res => {
            data = res.data
            return data // note - this will be an async function, be careful on front end writing things that depend on this state change for example
        })
        .catch(err => console.log(err))
}

export const getIDfromUsername = async (username) => {
    console.log(username)
    await axios.get(`/api/users/${username}`)
        .then(res => {
            return res.data[0].id
        })
        .catch(err => console.log(err))
}