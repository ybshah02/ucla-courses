import React, { useContext, createContext, useEffect, useState } from "react";
import {
    BrowserRouter as Router,
    Switch,
    Route,
    Link,
    Redirect,
    useHistory,
    useLocation
} from "react-router-dom";

import axios from "axios";

import { getIDfromUsername } from "./backend-calls";

/** For more details on
 * `authContext`, `ProvideAuth`, `useAuth` and `useProvideAuth`
 * refer to: https://usehooks.com/useAuth/
 */
const authContext = createContext();

export function ProvideAuth({ children }) {
    const auth = useProvideAuth();
    return (
        <authContext.Provider value={auth}>
            {children}
        </authContext.Provider>
    );
}

export const useAuth = () => {
    return useContext(authContext);
};


function useProvideAuth() {
    const [username, setUsername] = useState(null);
    const [password, setPassword] = useState(null);
    const [signedIn, setSignedIn] = useState(false)
    const [ID, setID] = useState(null)


    // Wrap any Firebase methods we want to use making sure ...
    // ... to save the user to state.
    const signin = async (usernamep, passwordp) => {
        setUsername(usernamep)
        setPassword(passwordp)
        setSignedIn(true)
    };

    const signOut = () => {
        setUsername(null)
        setPassword(null)
        setSignedIn(false)
    }

    /*    const signout = () => {
            return firebase
                .auth()
                .signOut()
                .then(() => {
                    setUser(false);
                });
        };*/

    // Subscribe to user on mount
    // Because this sets state in the callback it will cause any ...
    // ... component that utilizes this hook to re-render with the ...
    // ... latest auth object.

    useEffect(() => {

    }, []);
    // Return the user object and auth methods
    return {
        username,
        password,
        signin,
        signedIn,
        signOut,
        ID
    };
}
