import React, { useEffect, useState } from 'react';
import './Register.css';
import mainLogo from '../Files/bruinsource_logo.png'
import history from '../history.js'
import axios from 'axios';
import { Dialog, DialogTitle } from '@mui/material';

const Register = (props) => {
    const [username, setUsername] = useState(null)
    const [password, setPassword] = useState(null)
    const [email, setEmail] = useState(null)
    const [github, setGithub] = useState(null)
    const [languages, setLanguages] = useState(null)
    const [years, setYears] = useState(null)
    const [numProjects, setNumProjects] = useState(null)

    const [alert, setAlert] = useState(null)

    const [open, setOpen] = useState(false);

    const onSuccess = () => {
        setOpen(true)
        setTimeout(() => {
            setOpen(false)
            history.push('/')
        }, 2000)
    }


    const submitFunc = () => {
        const userInfo = {
            username: username,
            password: password,
            email: email,
            github: github,
            known_languages: languages,
            year_exp: years,
        }
        console.log('did this run?')
        axios.post('/api/register', userInfo)
            .then(err => {
                console.log(err)
                if (err.data.msg) {
                    if (err.data.msg === 'invalid_password') {
                        setAlert('Please enter a password that is at least 8 characters long, contains one uppercase letter, one number, and one special character.')
                    } else if (err.data.msg === 'invalid_email') {
                        setAlert('Please enter a valid email ending with .edu')
                    } else if (err.data.msg === 'something_wrong') {
                        setAlert('Something went wrong')
                    }
                } else if (err.data.constraint) {
                    if (err.data.constraint === 'users_un') {
                        setAlert("Username already exists in the database")
                    }
                } else {
                    onSuccess()
                }
            })
            .catch(err => {
                console.log(err)
                if (err.data.msg) {
                    if (err.data.msg === 'invalid_password') {
                        setAlert('Please enter a password that is at least 8 characters long, contains one uppercase letter, one number, and one special character.')
                    } else if (err.data.msg === 'invalid_email') {
                        setAlert('Please enter a valid email ending with .edu')
                    }
                } else {
                    if (err.data.constraint === 'users_un') {
                        setAlert("Username already exists in the database")
                    }
                }
            })
    }

    return (
        <div className="Register">
            <img src={mainLogo} className="MainLogo" alt="mainLogo" />
            <h2> Create a New Account</h2>
            <form className="Inputs">
                <div className="FormBox">
                    <input
                        type="text"
                        placeholder="Username..."
                        required
                        onChange={(input) => setUsername(input.target.value)}
                    />
                </div>
                <div className="FormBox">
                    <input
                        type="password"
                        placeholder="Password..."
                        required
                        onChange={(input) => setPassword(input.target.value)}
                    />
                </div>
                <div className="FormBox">
                    <input
                        type="email"
                        placeholder="yourname@website.com..."
                        required
                        onChange={(input) => setEmail(input.target.value)}
                    />
                </div>
                <div className="FormBox">
                    <input
                        type="url"
                        placeholder="http://github.com/..."
                        onChange={(input) => setGithub(input.target.value)}
                    />
                </div>
                <div className="FormBox">
                    <input
                        type="text"
                        placeholder="Known Languages (comma-separated)..."
                        onChange={(input) => setLanguages(input.target.value)}
                    />
                </div>
                <div className="FormBox">
                    <input
                        type="number"
                        min="0"
                        step="1"
                        placeholder="Years of Experience..."
                        onChange={(input) => setYears(input.target.value)}
                    />
                </div>
                <div>
                    <p id="error-text">
                        {alert}
                    </p>
                </div>

                <div className="Buttons">
                    <button type="button" className="BackToLogin" onClick={() => history.push('/')}>Back to Login</button>
                    <button type="button" onClick={submitFunc} className="CreateNewAccount">Create New Account</button>
                </div>
            </form>
            <Dialog open={open}>
                <DialogTitle>
                    Account successfully created! You may now sign in.
                </DialogTitle>
            </Dialog>
        </div>
    );
}

export default Register;