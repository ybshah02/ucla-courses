const { client, formatArrayToSql } = require('./db.js');

/**
 * Projects db representation
 * id: int
 * name: string
 * description: string
 * github: string
 * date_created: Date
 * last_updated: Date
 * author: int
 * collaborators: [int]
 */

function validateProjectName(projectName) {

    //check if project name doesn't have spaces
    const numSpaces = projectName
        .split('')
        .map(char => /\s/.test(char))
        .reduce((curr, prev) => curr + prev);

    if (numSpaces === 0) return true;
    else return false;

}

// create a project in db
function createProject(req, res) {
    const {
        name,
        description,
        tags,
        github,
        date_created,
        last_updated,
        author,
        collaborators,
        requests
    } = req.body;

    console.log(req.body)

    // let projectNameValid = validateProjectName(name);

    if (!name) {
        res.status(201).send({ msg: 'name_invalid' })
        return
    };

    formattedTags = formatArrayToSql(tags);
    formattedCollaboraters = formatArrayToSql(collaborators);
    formattedRequests = formatArrayToSql(requests);

    let finalAuthor = parseInt(author)

    if (author && name) {
        const query = 'INSERT INTO projects(name, description, tags, github, date_created, last_updated, author, collaborators) values($1, $2, $3::varchar[], $4, $5, $6, $7, $8::int[])';
        const vals = [name, description, formattedTags, github, date_created, last_updated, finalAuthor, collaborators];
        client
            .query(query, vals)
            .then(response => {
                res.send(response)
            })
            .catch(err => {
                console.log(err)
                res.status(201).send(err)
            });
    } else {
        res.status(201).send({ msg: 'author_invalid' });
    }
}

// search for a project
function searchProjects(req, res) {
    const { search } = req.params;
    const query = `select *, p.id as project_id from users u join projects p on u.id = p.author where p."name" like '%${search}%'`;
    client
        .query(query)
        .then(projects => {
            res.status(200).send(projects.rows)
        })
        .catch(err => {
            res.status(201).send(err)
        })
}

// delete project from db
function deleteProject(req, res) {
    const { id } = req.params;
    const query = `DELETE FROM projects p where p.id = '${id}'`;

    client
        .query(query)
        .catch(err => res.status(201).send({ msg: 'invalid_id' }));
}

// returns all active projects
function getProjects(req, res) {
    const query = `select *, p.id as project_id from users u join projects p on u.id = p.author`;
    client
        .query(query)
        .then(projects => {
            res.status(200).send(projects.rows)
        })
        .catch(err => res.status(201).send(err))
}

// returns project that correlates to id param
function getProjectById(req, res) {
    const { projectId } = req.params;
    const query = `select *, p.id as project_id from users u join projects p on u.id = p.author where p.id = '${projectId}'`;
    client
        .query(query)
        .then(projects => res.status(200).send(projects.rows[0]))
        .catch(err => res.status(201).send(err))
}

// returns projects being worked on by a user
async function getProjectsByUser(req, res) {
    const { username } = req.params;

    let userValid = false;
    const idQuery = `select * from users u where u.username = '${username}'`;
    let userId = null
    await client
        .query(idQuery)
        .then(user => {
            if (user.rowCount === 0) {
                res.status(201).send({ msg: `invalid_username` });
            }
            userValid = true;
            userId = user.rows[0].id;
        })
        .catch(() => {
            res.status(201).send({ msg: `invalid_username` });
            return
        })

    if (userValid) {
        const isAuthorQuery = `select *, p.id as project_id from users u join projects p on p.author = u.id where p.author = '${userId}' or '${userId}' = any (p.collaborators)`;
        client
            .query(isAuthorQuery)
            .then(projects => {
                res.status(200).send(projects.rows);
            })
            .catch(() => {
                res.status(201).send({ msg: `userId_invalid` })
            })
    }
}

// TODO: return all projects that contains at least one tag defined by user
function searchProjectsByTags(req, res) { 
    const tags = req.body.tags;

    const query = `select * from projects p join users u on p.author = u.id`;

    client
    .query(query)
    .then(projects => {
        let allProjects = projects.rows;
        let projectsWithTags = [];

        for (const p in allProjects){
            const project = allProjects[p];
            for (const t in tags){
                const tag = tags[t]
                if (project.tags.includes(tag)){
                     projectsWithTags.push(project);
                }
            }
        }
        res.status(200).send(projectsWithTags);
    })
    .catch(err => {
        console.log(err);
        res.status(201).send(err);
    })
}

// returns all collaboration requests for all projects
function getAllRequests(req, res) {
    const query = `select * from requests`;
    client
    .query(query)
    .then(requests => res.status(200).send(requests.rows))
    .catch(err => res.status(201).send(err));
}

// returns all collaboration requests made on a project
function getProjectRequests(req, res) {
    const { projectId } = req.params;
    const query = `select * from projects p where p.id = '${projectId}'`;
    client
        .query(query)
        .then(project => res.status(200).send(project.rows[0].requests))
        .catch(err => res.status(201).send(err));
}

function createRequest(req, res) {

    let { user, project_id, date_created } = req.body;
    
    if (typeof(project_id) === "string")
    {
        project_id = parseInt(project_id)
    }
    if (typeof(user) === "number") 
    {
        user = parseInt(user)
    }

    if (user && project_id){

        date_created = new Date();

        const query = `INSERT INTO requests(user_id, project_id, date_created) values($1, $2, $3)`;
        const vals = [user, project_id, date_created];

        client
        .query(query, vals)
        .catch(err => {
            console.log(err);
            res.status(201).send({ msg: 'too bad'});
        });
    } else {
        res.status(201).send({msg: 'invalid_input'});
    }
}

async function joinTeam(req, res) {

    const { userId, projectId, date_created } = req.body;

    if (typeof(userId) === "string")
    {
        userId = parseInt(userId)
    }
    let updatedCollaborators = null;
    const getProjectQuery = `select * from projects p where p.id = '${projectId}'`;
    await client
    .query(getProjectQuery)
    .then(project => {
        updatedCollaborators = project.rows[0].collaborators;
        updatedCollaborators.push(userId);
    })
    .catch(err => {
        console.log(err)
        res.status(201).send({msg: "u stink"});
    });
    let formattedCollaborators = formatArrayToSql(updatedCollaborators);
   const modifyUserQuery = `update projects set collaborators = '${formattedCollaborators}' where id = '${projectId}'`;
   client
    .query(modifyUserQuery)
    .catch(err => {
        console.log(err);
        res.status(201).send(err);
    });
}

async function deleteRequest(req, res) {
    
    const { userId, projectId } = req.body;

    const getProjectQuery = `select * from projects p where p.id = '${projectId}'`;
    await client
    .query(getProjectQuery)
    .then(project => {
        updatedCollaborators = project.rows[0].collaborators;
    })
    .catch(err => {
        console.log(err)
        res.status(201).send({msg: "u stink"});
    });
    if (typeof(userId) === "string") 
    {
        userId = parseInt(userId)
    }

    let result = updatedCollaborators.filter(element => element != userId)
    console.log(result)
    

    let formattedCollaborators = formatArrayToSql(result);

    const modifyUserQuery = `update projects set collaborators = '${formattedCollaborators}' where id = '${projectId}'`;
    client
     .query(modifyUserQuery)
     .catch(err => {
         console.log(err);
         res.status(201).send(err);
     });
}

module.exports = {
    createProject,
    deleteProject,
    getProjects,
    getProjectById,
    getProjectsByUser,
    getAllRequests,
    getProjectRequests,
    createRequest,
    joinTeam,
    deleteRequest,
    searchProjects,
    searchProjectsByTags,
}