import React, { useState } from 'react';
import AppBar from '@mui/material/AppBar';
import Box from '@mui/material/Box';
import Toolbar from '@mui/material/Toolbar';
import Typography from '@mui/material/Typography';
import Button from '@mui/material/Button';
import { useAuth } from '../Shared/ProvideAuth';
import history from '../history';
import DialogTitle from '@mui/material/DialogTitle';
import Dialog from '@mui/material/Dialog';


const Header = (props) => {

    const auth = useAuth()

    const buttonClick = () => {
        setOpen(true)
        setTimeout(() => {
            auth.signOut()
            setOpen(false)
            history.push('/')
        }, 2000)
    }

    const [open, setOpen] = useState(false)

    return (
        <React.Fragment>
            <Box>
                <AppBar position="static" style={{ background: "#005587" }}>
                    <Toolbar>
                        <Typography variant="h6" component="div" textAlign="left" fontFamily="Georgia" sx={{ flexGrow: 1 }}>
                            BruinSource
                        </Typography>
                        {!auth.signedIn ?
                            null
                            :
                            <Button style={{ marginRight: '3em' }} onClick={() => history.push('/searchinterest')} color="inherit" fontFamily="inherit">Search Projects by Tags</Button>
                        }
                        {!auth.signedIn ?
                            null
                            :
                            <Button style={{ marginRight: '3em' }} onClick={() => history.push('/dashboard')} color="inherit" fontFamily='inherit'>My Projects</Button>
                        }
                        {!auth.signedIn ?
                            null
                            :
                            <Button style={{ marginRight: '1em' }} onClick={buttonClick} color="inherit" fontFamily='inherit'>Log Out</Button>
                        }
                    </Toolbar>
                </AppBar>
            </Box>
            <Dialog open={open}>
                <DialogTitle>Signing you out...</DialogTitle>
            </Dialog>
        </React.Fragment>
    )
}



export default Header