import os

from cs50 import SQL
from flask import Flask, flash, jsonify, redirect, render_template, request, session

# Configure application
app = Flask(__name__)

# Ensure templates are auto-reloaded
app.config["TEMPLATES_AUTO_RELOAD"] = True

# Configure CS50 Library to use SQLite database
db = SQL("sqlite:///birthdays.db")


@app.after_request
def after_request(response):
    """Ensure responses aren't cached"""
    response.headers["Cache-Control"] = "no-cache, no-store, must-revalidate"
    response.headers["Expires"] = 0
    response.headers["Pragma"] = "no-cache"
    return response


@app.route("/", methods=["GET", "POST"])
def index():
    if request.method == "POST":
        try:
            name = request.form["name"]
            month = int(float(request.form["month"]))
            day = int(float(request.form["day"]))

            if 1 <= month <= 12 and 1 <= day <= 31:
                db.execute(
                    "INSERT INTO birthdays (name, month, day) VALUES(:name, :month, :day)",
                    name=name,
                    month=month,
                    day=day,
                )
        except:
            pass

        return redirect("/")

    else:
        birthday_data = db.execute("SELECT name, month, day FROM birthdays ORDER BY month, day")
        birthday_dict = dict()
        for person in birthday_data:
            birthday_dict[person["name"]] = f"{person['month']}/{person['day']}"

        return render_template("index.html", birthday_dict=birthday_dict)
