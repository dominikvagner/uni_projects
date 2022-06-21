#     ___ _   __
#    / _ \ | / /
#   / // / |/ /
#  /____/|___/
#
# @author Dominik Vagner, xvagne10
# @date 2022-04-17

Feature: Creation, editation and deletion of content

    Prducents should have the ability to create new content, edit it and delete it.
    # As an example, we will use the content of the "Use Case" type.
    # For other types tests would be fairly similiar. So to keep
    # the number of artifacts and tests to a reasonable amount.

    Background: Logged in as Producent
        Given I am logged in as Producent
            And There is a side menu

    Scenario: Access add new content page
        Given Use Cases page is displayed
        When I click on add new Use Case
        Then Add Use Case page is displayed

    Scenario: Create new content
        Given Add Use Case page is displayed
            And All the following fields are filled:
                | field                | entered_value |
                | Title                | Test Use Case |
                | Use Case Description | Test          |
        When I click on the Save button
        Then Confirmation of change appears
            And Specific Use Case page is displayed

    Scenario Outline: Create new content without filling out all the fields
        Given Add Use Case page is displayed
            And <required> fields are empty
        When I click on the Save button
        Then Error notification appears
            And Warnings around <required> fields are displayed
            Examples: Required fields
                | required             |
                | Title                |
                | Use Case Description |

    Scenario: Access edit content page
        Given Specific Use Case page is displayed
        When I click on the edit option
        Then Edit Use Case page is displayed

    Scenario: Edit content
        Given Edit Use Case page is displayed
            And All the following fields are filled:
                | field                | entered_value      |
                | Title                | Edited Title       |
                | Use Case Description | Edited Description |
        When I click on the Save button
        Then Confirmation of change appears
            And Specific Use Case page is displayed

    Scenario: Access deleteion dialog
        Given Specific Use Case page is displayed
        When I click on delete in actions
        Then Deletion dialog is displayed

    Scenario: Delete content
        Given Deletion dialog is displayed
        When I click on the Delete button
        Then Confirmation of change appears
            And Use Cases page is displayed
