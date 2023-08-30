const { client, formatArrayToSql } = require('./db.js')
const bcrypt = require('bcrypt')

/**
 * Users db representation
 * id: int
 * username: string
 * password: string
 * email: string
 * github: string
 * known_languages: [strings]
 * year_ex: int
 */

function validateEmail(email) {
    // check if email is not null and is associated with a university edu account 
    if (email.length != 0 && email.substring(email.length - 4) === '.edu') {
        return true;
    } else {
        return false;
    }
}

function validatePassword(password) {
    // check if password has at least one upper case character
    const numUpper = password
        .split('')
        .map(char => /[A-Z]/.test(char))
        .reduce((curr, prev) => curr + prev);

    // check if password has at least one special case character
    const numSpecial = password
        .split('')
        .map(char => /[^a-zA-Z\d]/.test(char))
        .reduce((curr, prev) => curr + prev);

    // check i password has at least one number
    const numDigits = password
        .split('')
        .map(char => /\d/.test(char))
        .reduce((curr, prev) => curr + prev);

    if (password.length >= 8 && numUpper > 0 && numSpecial > 0 && numDigits > 0) return true;
    else return false;
}

// adds a user to database and returns the user object in json 
async function registerUser(req, res) {
    let { username, 
            password, 
            email, 
            github, 
            year_exp, 
            known_languages
    } = req.body;

    let usernameValid = true
    if (username == '') {
        usernameValid = false
    }

    // validate password input
    let passwordValid = validatePassword(password);
    if (!passwordValid) {
        res.status(201).send({ msg: 'invalid_password' })
        return;
    }

    // validate email input
    let emailValid = validateEmail(email);
    if (!emailValid) {
        res.status(201).send({ msg: 'invalid_email' })
        return;
    }


    // set default to empty string
    if (!github) github = '';

    // set default to 0 years of experience if no input
    if (!year_exp) year_exp = 0;

    known_languages_input = formatArrayToSql(known_languages);

    if (!known_languages) {
        res.status(201).send({ msg: 'invalid_languages' })
        return;
    }
    known_languages_input = formatArrayToSql(known_languages);
   
    // make query if inputs are all valid
    if (emailValid && passwordValid) { 
        const query = 'INSERT INTO users(username, password, email, github, year_exp, known_languages) values($1, $2, $3, $4, $5, $6::varchar[])';
        const vals = [username, password, email, github, year_exp, known_languages_input];
        client
            .query(query, vals)
            .then(response => res.status(200).send(response))
            .catch(err => res.status(201).send(err));
    } else {
        res.status(201).send({msg: 'error'});
    }
}

function validateLogin(req, res) {
    const { username, password } = req.body;
    const query = `select * from users u where u.username = '${username}'`;

    client
        .query(query)
        .then(user => {
            if (password === user.rows[0].password) {
                res.status(200).send({msg: 'success'})
            } else {
                res.status(201).send({ msg: 'invalid_password' })
            }
        })
        .catch(err => {
            res.status(201).send({ msg: 'invalid_username' })
        })
}

// deletes a user from database and returns that user object in json
function deleteUser(req, res) {
    const { username } = req.params;
    const query = `DELETE FROM users u where u.username = '${username}'`;

    client
        .query(query)
        .catch(err => res.status(201).send({ msg: 'invalid_username' }));
}

// returns json of all existing users 
function getUsers(req, res) {
    const query = `select * from users`;
    client
        .query(query)
        .then(users => {
            res.status(200).send(users.rows);
        })
        .catch(err => res.status(201).send(err));
}

// returns json of user found by given username
function getUserByUsername(req, res) {
    const { username } = req.params;
    const query = `select * from users u where u.username = '${username}'`;
    client
        .query(query)
        .then(user => {
            res.status(200).send(user.rows[0]);
        })
        .catch(err => {
            res.status(201).send({ msg: `invalid_username` });
        })
}

function getUserByID(req, res) {
    const { id } = req.params;
    const query = `select * from users u where u.id = ${id}`;
    client.query(query)
    .then(user => {
        console.log(user)
        res.status(200).send(user.rows[0])
    })
    .catch(err => {
        console.log(err)
        res.status(201).send({msg: 'error'})
    })
}

module.exports = {
    registerUser,
    formatArrayToSql,
    validateLogin,
    getUsers,
    getUserByUsername,
    deleteUser,
    getUserByID
}