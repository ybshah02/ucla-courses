/*
const {client, formatArrayToSql} = require('./db.js')

class Task {
    constructor(id, name, description, tags, status, project_id, date_created, date_due, date_finished, assigned_to){
        this.id = id;
        this.name = name;
        this.description = description;
        this.tags = tags;
        this.status = status;
        this.project_id = project_id;
        this.date_created = date_created;
        this.date_due = date_due;
        this.date_finished = date_finished;
        this.assigned_to = assigned_to;
    }
}

// TODO: query add functionality 
function createTask(req, res){

    const { name, description, tags, project_id, assigned_user, date_created, date_due, is_finished, date_finished } = req.body;

    formattedTags = formatArrayToSql(tags);

    if (name){

        date_created = new Date();

        const query = `INSERT INTO tasks(name, description, tags, project_id, assigned_user, date_created, date_due, is_finished, date_finished) values($1, $2, $3::varchar[], $4, $5, $6, $7, $8, $9)`;
        const vals = [name, description, formattedTags, project_id, assigned_user, date_created, date_due, is_finished, date_finished];

        client
        .query(query, vals)
        .catch(err => {
            console.log(err);
            res.status(201).send(err);
        });
    } else {
        res.status(201).send({msg: 'invalid_input'});
    }

}

function deleteTask(req, res){
    const { taskId } = req.param;

    const query = `delete from tasks t where t.id = '${taskId}'`;
    client
    .query(query)
    .catch(err => {
        console.log(err);
        res.status(201).send(err);
    });
}

function getTasks(req, res) {
    const { projectId } = req.param;
    const query = `select * from tasks t where t.project_id = '${projectId}'`;
    client
    .query(query)
    .then(tasks => {
        res.status(200).send(tasks.rows);
    })
    .catch(() => {
        res.status(201).send({msg: `invalid_project_id`});
    })
}

module.exports = {
    createTask,
    deleteTask,
    getTasks
}
*/