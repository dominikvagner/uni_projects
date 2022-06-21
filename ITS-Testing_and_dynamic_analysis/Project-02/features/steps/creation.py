import time
from behave import *
from selenium.webdriver.common.by import By
from selenium.webdriver.common.keys import Keys
from selenium.common.exceptions import NoSuchElementException


###############################################  Background  ##########################################################
@given(u'There is a side menu')
def step_impl(context):
    context.driver.find_element(By.CSS_SELECTOR, ".plone-toolbar-container")


@given(u'I am logged in as Producent')
def step_impl(context):
    context.driver.find_element_by_tag_name('body').send_keys(Keys.HOME)
    context.driver.find_element(By.ID, "personaltools-login").click()
    context.driver.find_element(By.ID, "__ac_name").send_keys("itsadmin")
    context.driver.find_element(By.ID, "__ac_password").send_keys("itsadmin")
    login_btn = None
    try:
        login_btn = context.driver.find_element(By.XPATH,
                                                "//div[@class='pattern-modal-buttons']//input[@id='buttons-login']")
    except NoSuchElementException:
        login_btn = context.driver.find_element(By.XPATH,
                                                "//input[@id='buttons-login']")
    time.sleep(2)
    context.driver.execute_script("arguments[0].click();", login_btn)
    time.sleep(2)


###############################################  1. Scenario  #########################################################
@given(u'Use Cases page is displayed')
def step_impl(context):
    context.driver.get("http://localhost:8080/repo/use-cases")


@when(u'I click on add new Use Case')
def step_impl(context):
    context.driver.find_element_by_tag_name('body').send_keys(Keys.HOME)
    add_new_menu = context.driver.find_element(By.ID, "plone-contentmenu-factories")
    context.driver.execute_script("arguments[0].click();", add_new_menu)
    time.sleep(1)
    add_uc = context.driver.find_element(By.XPATH, "//a[@id='use_case']")
    context.driver.execute_script("arguments[0].click();", add_uc)
    time.sleep(1)


@then(u'Add Use Case page is displayed')
def step_impl(context):
    cur_url = context.driver.current_url
    assert cur_url == "http://localhost:8080/repo/use-cases/++add++use_case"


###############################################  2. Scenario  #########################################################
@given(u'Add Use Case page is displayed')
def step_impl(context):
    context.driver.get("http://localhost:8080/repo/use-cases/++add++use_case")


@given(u'All the following fields are filled')
def step_impl(context):
    select = context.driver.find_element(By.ID, "form-widgets-use_case_description_text_format")
    for option in select.find_elements(By.TAG_NAME, 'option'):
        if option.text == "text/x-web-textile":
            option.click()
            break

    title = context.table[0][1]
    description = context.table[1][1]
    context.driver.find_element(By.ID, "form-widgets-IBasic-title").clear()
    context.driver.find_element(By.NAME, "form.widgets.use_case_description").clear()
    context.driver.find_element(By.ID, "form-widgets-IBasic-title").send_keys(title)
    context.driver.find_element(By.NAME, "form.widgets.use_case_description").send_keys(description)


###############################################  3. Scenario  #########################################################
@given(u'Title fields are empty')
def step_impl(context):
    title_field = context.driver.find_element(By.ID, "form-widgets-IBasic-title").clear()


@given(u'Use Case Description fields are empty')
def step_impl(context):
    select = context.driver.find_element(By.ID, "form-widgets-use_case_description_text_format")
    for option in select.find_elements(By.TAG_NAME, 'option'):
        if option.text == "text/x-web-textile":
            option.click()
            break

    description_field = context.driver.find_element(By.NAME, "form.widgets.use_case_description").clear()


@then(u'Error notification appears')
def step_impl(context):
    context.driver.find_element(By.XPATH, "//dl[@class='portalMessage error']")


@then(u'Warnings around Title fields are displayed')
def step_impl(context):
    context.driver.find_element(By.XPATH,
                                "//div[@id='formfield-form-widgets-IBasic-title']//div[@class='fieldErrorBox']")


@then(u'Warnings around Use Case Description fields are displayed')
def step_impl(context):
    context.driver.find_element(By.XPATH,
                                "//div[@id='formfield-form-widgets-use_case_description']//div[@class='fieldErrorBox']")


###############################################  4. Scenario  #########################################################
@given(u'New Use Case page is displayed')
def step_impl(context):
    context.driver.get("http://localhost:8080/repo/use-cases")
    context.driver.find_element(By.LINK_TEXT, "Test Use Case").click()


@when(u'I click on the edit option')
def step_impl(context):
    context.driver.find_element(By.ID, "contentview-edit").click()


@then(u'Edit Use Case page is displayed')
def step_impl(context):
    context.driver.current_url.startswith("http://localhost:8080/repo/use-cases/test-use-case/edit")


###############################################  4. Scenario  #########################################################
@given(u'Edit new Use Case page is displayed')
def step_impl(context):
    context.driver.get("http://localhost:8080/repo/use-cases")
    context.driver.find_element(By.LINK_TEXT, "Test Use Case").click()
    context.driver.find_element(By.ID, "contentview-edit").click()


###############################################  6. Scenario  #########################################################
@given(u'Edited Use Case page is displayed')
def step_impl(context):
    context.driver.get("http://localhost:8080/repo/use-cases")
    context.driver.find_element(By.LINK_TEXT, "Edited Title").click()


@when(u'I click on delete in actions')
def step_impl(context):
    context.driver.find_element_by_tag_name('body').send_keys(Keys.HOME)
    actions = context.driver.find_element(By.ID, "plone-contentmenu-actions")
    context.driver.execute_script("arguments[0].click();", actions)
    time.sleep(1)
    delete = context.driver.find_element(By.XPATH, "//a[@id='plone-contentmenu-actions-delete']")
    context.driver.execute_script("arguments[0].click();", delete)
    time.sleep(1)


@then(u'Deletion dialog is displayed')
def step_impl(context):
    context.driver.find_element(By.XPATH, "//h2[normalize-space()='Deleting overview']")


###############################################  7. Scenario  #########################################################
@given(u'Deletion dialog is displayed')
def step_impl(context):
    context.driver.get("http://localhost:8080/repo/use-cases")
    context.driver.find_element(By.LINK_TEXT, "Edited Title").click()
    context.driver.find_element_by_tag_name('body').send_keys(Keys.HOME)
    actions = context.driver.find_element(By.ID, "plone-contentmenu-actions")
    context.driver.execute_script("arguments[0].click();", actions)
    time.sleep(1)
    delete = context.driver.find_element(By.XPATH, "//a[@id='plone-contentmenu-actions-delete']")
    context.driver.execute_script("arguments[0].click();", delete)
    time.sleep(1)


@when(u'I click on the Delete button')
def step_impl(context):
    delete_btn = context.driver.find_element(By.ID, "form-buttons-Delete")
    context.driver.execute_script("arguments[0].click();", delete_btn)


@then(u'Use Cases page is displayed')
def step_impl(context):
    time.sleep(3)
    cur_url = context.driver.current_url
    assert cur_url == "http://localhost:8080/repo/use-cases#c4=all&b_start=0"
