import * as React from 'react';
import Table from '@mui/material/Table';
import TableBody from '@mui/material/TableBody';
import TableCell from '@mui/material/TableCell';
import TableContainer from '@mui/material/TableContainer';
import TableHead from '@mui/material/TableHead';
import TableRow from '@mui/material/TableRow';
import Paper from '@mui/material/Paper';
import './ProjectTable.css'
import Typography from '@mui/material/Typography';
import {makeStyles} from '@mui/styles';
import Button from '@mui/material/Button';
import history from '../history';
import axios from 'axios';

const useStyles = makeStyles({
  custom: {
    color: "#005587",
    fontWeight: "bold",
    fontFamily: "Georgia"
  }
});

const useStyles2 = makeStyles({
  custom: {
    color: "black",
    fontFamily: "Georgia"
  }
});

export default function ProjectTable(props) {

  // must parse input data

  const { data } = props;

  const classes = useStyles();

  const classes2 = useStyles2();

  const renderTableRows = () => {
    return data.map((project, index) => {
      const { id, name, description, tags, date_created, last_updated, author, collaborators, requests, username, project_id } = project

      var d = new Date(date_created)
      d = d.toDateString()
      var collaboratorsExist = false
      if (collaborators) {
          collaboratorsExist = true
      }

      const onClickFunction = () => {
        console.log(id)
        history.push('/projectinfo', [id])
      }

      let formattedTags = '';

      /* console.log(tags) */

      tags.forEach(val => {
        if (tags.indexOf(val) !== tags.length - 1 ){
          formattedTags += val + ', ';
        } else {
          formattedTags += val;
        }
      });

      return (
        <TableRow
          key={index}
          sx={{ '&:last-child td, &:last-child th': { border: 0 } }}
        >
          <TableCell component="th" scope="row" className={classes2.custom}>
            {name}
          </TableCell>
          <TableCell align="right" className={classes2.custom}>{project.username}</TableCell>
          <TableCell align="right" className={classes2.custom}>{d}</TableCell>
          <TableCell align="right" className={classes2.custom}>{formattedTags}</TableCell>
          <TableCell align="right" className={classes2.custom}>{collaboratorsExist ? collaborators : 'No collaborators'}</TableCell>
          <TableCell align="right"><Button onClick={onClickFunction}>View</Button></TableCell>
        </TableRow>);
    })
  }

  return (
    <div className="ProjectTableContainer">
      <TableContainer component={Paper}>
        <Table sx={{ minWidth: 650 }} aria-label="simple table">
          <TableHead>
            <TableRow>
              <TableCell><Typography variant="h6" color="inherit" className={classes.custom}>Project Name</Typography></TableCell>
              <TableCell align="right"><Typography variant="h6" color="inherit" className={classes.custom}>Author</Typography></TableCell>
              <TableCell align="right"><Typography variant="h6" color="inherit" className={classes.custom}>Date Created</Typography></TableCell>
              <TableCell align="right"><Typography variant="h6" color="inherit" className={classes.custom}>Tags</Typography></TableCell>
              <TableCell align="right"><Typography variant="h6" color="inherit" className={classes.custom}>Collaborators</Typography></TableCell>
              <TableCell></TableCell>
            </TableRow>
          </TableHead>
          <TableBody>
            {renderTableRows()}
          </TableBody>
        </Table>
      </TableContainer>
    </div>
  );
}
