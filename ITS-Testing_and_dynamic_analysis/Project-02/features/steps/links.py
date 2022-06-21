import time
from behave import *
from selenium.webdriver.common.by import By
from selenium.webdriver.common.keys import Keys
from selenium.common.exceptions import NoSuchElementException


# 1. scenario
@then(u'Specific Use Case page is displayed')
@given(u'Specific Use Case page is displayed')
def step_impl(context):
    context.driver.get("http://localhost:8080/repo/use-cases")
    context.driver.find_element(By.LINK_TEXT, "UC1 - Intelligent Traffic Surveillance").click()


@given(u'It has a link to organization')
def step_impl(context):
    context.driver.find_element(By.ID, "form-widgets-partners")


@when(u'I click on the link to organization')
def step_impl(context):
    context.driver.find_element(By.XPATH, "//span[@title='organization']").click()


@then(u'Specific Organization page is displayed')
def step_impl(context):
    context.driver.current_url.startswith("http://localhost:8080/repo/organizations/")


# 2. scenario
@given(u'Edit Use Case page is displayed')
def step_impl(context):
    context.driver.get("http://localhost:8080/repo/use-cases")
    context.driver.find_element(By.LINK_TEXT, "UC1 - Intelligent Traffic Surveillance").click()
    context.driver.find_element(By.ID, "contentview-edit").click()


@given(u'The partners field has a single partner')
def step_impl(context):
    partners_len = len(context.driver.find_elements(By.XPATH,
                                                    "//div[@id='s2id_autogen3']//li[@class='select2-search-choice']"))
    assert partners_len == 1


@when(u'I remove the partner')
def step_impl(context):
    context.driver.find_element_by_tag_name('body').send_keys(Keys.PAGE_DOWN)
    context.driver.find_element(By.XPATH, "//div[@id='s2id_autogen3']//a[@class='select2-search-choice-close']").click()
    context.driver.find_element(By.XPATH, "//label[normalize-space()='Use Case Provider']").click()


@when(u'I click on the Save button')
def step_impl(context):
    context.driver.find_element_by_tag_name('body').send_keys(Keys.END)
    context.driver.find_element(By.ID, "form-buttons-save").click()


@then(u'Confirmation of change appears')
def step_impl(context):
    context.driver.find_element(By.XPATH, "/html/body/div[1]/div[3]/div/aside/div[1]")


@then(u'The partners section is empty')
def step_impl(context):
    empty = False
    try:
        context.driver.find_element(By.ID, "form-widgets-partners")
    except NoSuchElementException:
        empty = True
    assert empty is True


# 3. scenario
@given(u'The partners field is empty')
def step_impl(context):
    partners_len = len(context.driver.find_elements(By.XPATH,
                                                    "//div[@id='s2id_autogen3']//li[@class='select2-search-choice']"))
    assert partners_len == 0


@when(u'I add the following organization in partners field')
def step_impl(context):
    context.driver.find_element_by_tag_name('body').send_keys(Keys.PAGE_DOWN)
    context.driver.find_element(By.XPATH,
                                "//div[@id='formfield-form-widgets-partners']//span[@class='glyphicon glyphicon-home']"
                                ).click()
    context.driver.find_element_by_tag_name('body').send_keys(Keys.TAB)
    context.driver.find_element(By.XPATH, "//div[@id='s2id_autogen3']//li[@class='select2-search-field']").click()
    time.sleep(2)
    for option in context.table:
        context.driver.find_element_by_tag_name('body').send_keys(option[0])
        time.sleep(2)
        context.driver.find_element_by_tag_name('body').send_keys(Keys.TAB)


@then(u'The partners section isn\'t empty')
def step_impl(context):
    partners_len = len(context.driver.find_elements(By.ID, "form-widgets-partners"))
    assert partners_len > 0
