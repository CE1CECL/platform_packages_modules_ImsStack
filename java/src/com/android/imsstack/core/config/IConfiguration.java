/*
    Author
    <table>
    date      author                        description
    --------  --------------                ----------
    20101104  hwangoo.park@                 Created
    </table>

    Description
*/

package com.android.imsstack.core.config;

public interface IConfiguration
{
    /**
     * Returns all the tables for the current configuration
     */
    public String[] getTables();

    /**
     * Returns the specified table contents
     * @param name to be retrieved
     */
    public String[][] getTableContent(String name);
};
