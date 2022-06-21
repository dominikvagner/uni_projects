# ITS Projekt 1

- **Autor:** Dominik Vágner (xvagne10)
- **Datum:** 2022-04-17

## Matice pokrytí artefaktů

Popis artefaktů:

| Artifact Id (AId) | Artifact Description                                      |
| ----------------- | --------------------------------------------------------- |
| P\_1              | Web page for viewing list of use cases.                   |
| P\_2              | Web page for viewing a specific use case.                 |
| P\_3              | Web page for adding a new use case.                       |
| P\_4              | Web page for editing a specific use case.                 |
| P\_5              | Web popup for deleting a specific use case.               |
| P\_6              | Web page for viewing organization.                        |
| A\_1              | Activity for clicking the save button.                    |
| A\_2              | Activity for clicking the delete button.                  |
| A\_3              | Activity for filling fields in use case edit.             |
| A\_4              | Activity for adding a partner when editing use case.      |
| A\_5              | Activity for removing a partner when editing use case.    |
| A\_6              | Activity for clicking a link to organization.             |
| A\_7              | Activity for clicking a edit in side menu.                |
| A\_8              | Activity for clicking a add new use case in side menu.    |
| A\_9              | Activity for changing the published state option.         |
| A\_10             | Activity for clicking a delete action in side menu.       |
| C\_1              | Check for being a producent.                              |
| C\_2              | Check for being a consumer.                               |
| C\_3              | Check for amount of links in partner section of use case. |
| C\_4              | Check for type of published state of use case in list.    |
| C\_5              | Check for amount of partners when editing use case.       |
| C\_6              | Check for confirmation of change.                         |
| C\_7              | Check for error message.                                  |
| C\_8              | Check for empty fields when creating new use case.        |

Matice pokrytí:

| AId   | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 10 | 11 | 12 | 13 |
| ----- | - | - | - | - | - | - | - | - | - | -- | -- | -- | -- |
| P\_1  | x |   |   |   |   |   | x |   |   |    | x  | x  |    |
| P\_2  |   | x |   | x | x | x |   | x | x | x  |    |    | x  |
| P\_3  | x | x | x |   |   |   |   |   |   |    |    |    |    |
| P\_4  |   |   |   | x | x |   |   |   | x | x  |    |    |    |
| P\_5  |   |   |   |   |   | x | x |   |   |    |    |    |    |
| P\_6  |   |   |   |   |   |   |   | x |   |    |    |    |    |
| A\_1  |   | x | x |   | x |   |   |   | x | x  |    |    |    |
| A\_2  |   |   |   |   |   |   | x |   |   |    |    |    |    |
| A\_3  |   | x |   |   | x |   |   |   |   |    |    |    |    |
| A\_4  |   |   |   |   |   |   |   |   | x |    |    |    |    |
| A\_5  |   |   |   |   |   |   |   |   |   | x  |    |    |    |
| A\_6  |   |   |   |   |   |   |   | x |   |    |    |    |    |
| A\_7  |   |   |   | x |   |   |   |   |   |    |    |    |    |
| A\_8  | x |   |   |   |   |   |   |   |   |    |    |    |    |
| A\_9  |   |   |   |   |   |   |   |   |   |    |    |    | x  |
| A\_10 |   |   |   |   |   | x |   |   |   |    |    |    |    |
| C\_1  | x | x | x | x | x | x | x |   | x | x  |    | x  | x  |
| C\_2  |   |   |   |   |   |   |   |   |   |    | x  |    |    |
| C\_3  |   |   |   |   |   |   |   | x | x | x  |    |    |    |
| C\_4  |   |   |   |   |   |   |   |   |   |    | x  | x  |    |
| C\_5  |   |   |   |   |   |   |   |   | x | x  |    |    |    |
| C\_6  |   | x |   |   | x |   | x |   | x | x  |    |    | x  |
| C\_7  |   |   | x |   |   |   |   |   |   |    |    |    |    |
| C\_8  |   |   | x |   |   |   |   |   |   |    |    |    |    |


## Matice Feature-Test

| Feature file     | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 10 | 11 | 12 | 13 |
| ---------------- | - | - | - | - | - | - | - | - | - | -  | -- | -- | -- |
| creation.feature | x | x | x | x | x | x | x |   |   |    |    |    |    |
| links.feature    |   |   |   |   |   |   |   | x | x | x  |    |    |    |
| state.feature    |   |   |   |   |   |   |   |   |   |    | x  | x  | x  |
