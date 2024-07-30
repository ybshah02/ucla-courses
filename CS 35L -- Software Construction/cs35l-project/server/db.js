const { Client } = require('pg');
const { query } = require('express');
const { resolveSoa } = require('dns');
require('dotenv').config()

// create global client object that links postgres to node js
const client = new Client({
    connectionString: process.env.DB_STRING,
    ssl: {
        rejectUnauthorized: false
    }
});

function connectdb() {
    // connect to postgres
    console.log('Connecting to Postgres...');
    client.connect().catch(err => console.log(err))
}

// format array worked into sql type
function formatArrayToSql(arr) {
    let splitArr = null;

    if (!arr) {
        return null
    }
    
    if (arr.includes(',')) {
        splitArr = arr.split(',')
        arr.map(each => {
            formatted += (each + ',');
        });
        formatted = formatted.substring(0, formatted.length - 1);
        formatted += '}';
    } else {
        splitArr = '{' + arr + '}'
    }
    return splitArr;
}

module.exports = {
    client,
    connectdb,
    formatArrayToSql,
}