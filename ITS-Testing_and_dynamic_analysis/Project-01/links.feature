#     ___ _   __
#    / _ \ | / /
#   / // / |/ /
#  /____/|___/
#
# @author Dominik Vagner, xvagne10
# @date 2022-04-17

Feature: Addition, change and deletion of links between content

    Different types of content can have links/bindings between each other.
    Through theese links users can get to other types of content.
    Producents should be able to add, change and delete them.
    # As an example, we will use the binding bewteen Use Case and organization.
    # For other types tests would be fairly similiar. So to keep
    # the number of artifacts and tests to a reasonable amount.

    # Changing links, is pretty much just creating and deleting them so it's covered by testing them.

    Scenario: Going from one content to another.
        Given Specific Use Case page is displayed
            And It has a link to organization
        When I click on the link to organization
        Then Specific Organization page is displayed

    Scenario: Adding a link between content
        Given I am logged in as Producent
            And Edit Use Case page is displayed
            And The partners field is empty
        When I add the following organization in partners field:
            | organization                  |
            | Brno University of Technology |
            And I click on the Save button
        Then Confirmation of change appears
            And Specific Use Case page is displayed
            And The partners section isn't empty

    Scenario: Deleting a link between content
        Given I am logged in as Producent
            And Edit Use Case page is displayed
            And The partners field has a single partner
        When I remove the partner
            And I click the Save button
        Then Confirmation of change appears
            And Specific Use Case page is displayed
            And The partners section is empty
