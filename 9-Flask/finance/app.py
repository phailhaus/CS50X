import os

from cs50 import SQL
from flask import Flask, flash, redirect, render_template, request, session
from flask_session import Session
from werkzeug.security import check_password_hash, generate_password_hash

from helpers import apology, login_required, lookup, usd

# Configure application
app = Flask(__name__)

# Custom filter
app.jinja_env.filters["usd"] = usd

# Configure session to use filesystem (instead of signed cookies)
app.config["SESSION_PERMANENT"] = False
app.config["SESSION_TYPE"] = "filesystem"
Session(app)

# Configure CS50 Library to use SQLite database
db = SQL("sqlite:///finance.db")


def generate_page(request_type, request_contents):
    # Query database for user cash
    rows = db.execute("SELECT cash FROM users WHERE id = ?", session["user_id"])
    cash = rows[0]["cash"]

    # Query database to get owned stocks for the user
    rows = db.execute(
        "SELECT * FROM stocks WHERE user_id = :user_id",
        user_id=session["user_id"],
    )

    account_value = cash
    stock_list = list()

    if len(rows) > 0:
        for row in rows:
            symbol = row["symbol"]
            shares = row["shares"]
            try:
                current_price = lookup(row["symbol"])["price"]
            except:
                current_price = 0
            current_value = shares * current_price
            profit = current_value - row["total_paid"]

            symbol_dict = {
                "symbol": symbol.upper(),
                "shares": shares,
                "current_price": current_price,
                "current_value": current_value,
                "profit": profit,
            }

            stock_list.append(symbol_dict)
            account_value += current_value

        stock_list.sort(key=lambda x: x["current_value"], reverse=True)

    cash_dict = {
        "symbol": "Cash",
        "shares": "-",
        "current_price": "-",
        "current_value": cash,
        "profit": "-",
    }

    stock_list.insert(0, cash_dict)

    for stock in stock_list:
        for key in ("current_price", "current_value", "profit"):
            if type(stock[key]) != str:
                stock[key] = f"${stock[key]:,.2f}"

    return render_template(
        "index.html",
        account_value=f"{account_value:,.2f}",
        stock_list=stock_list,
        request_type=request_type,
        request_contents=request_contents,
    )


@app.after_request
def after_request(response):
    """Ensure responses aren't cached"""
    response.headers["Cache-Control"] = "no-cache, no-store, must-revalidate"
    response.headers["Expires"] = 0
    response.headers["Pragma"] = "no-cache"
    return response


@app.route("/")
@login_required
def index():
    """Show portfolio of stocks"""
    # User reached route via POST (as by submitting a form via POST)
    if request.method == "POST":
        return apology("TODO")

    # User reached route via GET (as by clicking a link or via redirect)
    else:
        return generate_page("index", None)


@app.route("/buy", methods=["GET", "POST"])
@login_required
def buy():
    """Buy shares of stock"""
    # User reached route via POST (as by submitting a form via POST)
    if request.method == "POST":
        try:
            symbol = request.form["symbol"]
            shares = request.form["shares"]
            if shares.isdigit() and shares != "0":
                shares = int(float(shares))
            else:
                return apology("Invalid Input", code=400)

            stock_quote = lookup(symbol)
            price = stock_quote["price"]
            extension = shares * price

            # Query database for user cash
            rows = db.execute("SELECT * FROM users WHERE id = ?", session["user_id"])
            cash = rows[0]["cash"]

            if extension <= cash:
                # Update database to deduct spent cash
                db.execute(
                    "UPDATE users SET cash = :new_cash WHERE id = :id",
                    new_cash=cash - extension,
                    id=session["user_id"],
                )

                # Query database to see if user already owns stock
                rows = db.execute(
                    "SELECT * FROM stocks WHERE user_id = :user_id AND symbol = :symbol",
                    user_id=session["user_id"],
                    symbol=symbol,
                )

                # If the user owns the stock, update that entry to add more
                if len(rows) > 0:
                    rows = db.execute(
                        "UPDATE stocks SET shares = :set_shares, total_paid = :total_paid WHERE user_id = :user_id AND symbol = :symbol",
                        user_id=session["user_id"],
                        symbol=symbol,
                        set_shares=rows[0]["shares"] + shares,
                        total_paid=rows[0]["total_paid"] + extension,
                    )
                # If user doesn't own the stock, add an entry for it
                else:
                    db.execute(
                        "INSERT INTO stocks (user_id, symbol, shares, total_paid) VALUES(:user_id, :symbol, :shares, :total_paid)",
                        user_id=session["user_id"],
                        symbol=symbol,
                        shares=shares,
                        total_paid=extension,
                    )

                # Create history entry
                db.execute(
                    "INSERT INTO history (user_id, symbol, shares, price) VALUES(:user_id, :symbol, :shares, :price)",
                    user_id=session["user_id"],
                    symbol=symbol,
                    shares=shares,
                    price=price,
                )

                return generate_page(
                    "quote-message", f"Bought {shares} shares of {symbol}."
                )
            else:
                return generate_page(
                    "quote-message",
                    f"Cound not buy {shares} shares of {symbol}. Could not afford.",
                )
        except:
            return apology("Invalid Input", code=400)

    # User reached route via GET (as by clicking a link or via redirect)
    else:
        return generate_page("index", None)


@app.route("/history")
@login_required
def history():
    """Show history of transactions"""
    # User reached route via POST (as by submitting a form via POST)
    if request.method == "POST":
        return apology("TODO")

    # User reached route via GET (as by clicking a link or via redirect)
    else:
        # Query database to get owned stocks for the user
        rows = db.execute(
            "SELECT * FROM history WHERE user_id = :user_id",
            user_id=session["user_id"],
        )

        history_list = list()

        if len(rows) > 0:
            for row in rows:
                profit = row["profit"]
                if profit is None:
                    profit = "N/A"

                history_dict = {
                    "time": row["time"],
                    "symbol": row["symbol"],
                    "shares": row["shares"],
                    "price": row["price"],
                    "extension": row["shares"] * row["price"],
                    "profit": profit,
                }

                history_list.append(history_dict)

        for entry in history_list:
            for key in ("price", "extension", "profit"):
                if type(entry[key]) != str:
                    entry[key] = f"${entry[key]:,.2f}"

        history_list.reverse()

        return render_template("history.html", history_list=history_list)


@app.route("/login", methods=["GET", "POST"])
def login():
    """Log user in"""

    # Forget any user_id
    session.clear()

    # User reached route via POST (as by submitting a form via POST)
    if request.method == "POST":
        # Ensure username was submitted
        if not request.form.get("username"):
            return apology("must provide username", 403)

        # Ensure password was submitted
        elif not request.form.get("password"):
            return apology("must provide password", 403)

        # Query database for username
        rows = db.execute(
            "SELECT * FROM users WHERE username = ?", request.form.get("username")
        )

        # Ensure username exists and password is correct
        if len(rows) != 1 or not check_password_hash(
            rows[0]["hash"], request.form.get("password")
        ):
            return apology("invalid username and/or password", 403)

        # Remember which user has logged in
        session["user_id"] = rows[0]["id"]

        # Redirect user to home page
        return redirect("/")

    # User reached route via GET (as by clicking a link or via redirect)
    else:
        return render_template("login.html")


@app.route("/logout")
def logout():
    """Log user out"""

    # Forget any user_id
    session.clear()

    # Redirect user to login form
    return redirect("/")


@app.route("/quote", methods=["GET", "POST"])
@login_required
def quote():
    """Get stock quote."""
    # User reached route via POST (as by submitting a form via POST)
    if request.method == "POST":
        try:
            symbol = request.form["symbol"]
            stock_quote = lookup(symbol)
            stock_quote["price"] = f'{stock_quote["price"]:,.2f}'
            if type(stock_quote) == dict:
                return generate_page("quote", stock_quote)
            else:
                return apology("Invalid Input", code=400)
        except:
            return apology("Invalid Input", code=400)

    # User reached route via GET (as by clicking a link or via redirect)
    else:
        return generate_page("index", None)


@app.route("/register", methods=["GET", "POST"])
def register():
    """Register user"""
    # User reached route via POST (as by submitting a form via POST)
    if request.method == "POST":
        try:
            username = request.form["username"]
            password = request.form["password"]
            confirmation = request.form["confirmation"]
            if password != confirmation:
                return apology("Password/Confirmation\nMismatch", code=400)
            if username == "" or password == "":
                return apology("Username or Password\nBlank", code=400)
            usercount = len(
                db.execute("SELECT username FROM users WHERE username = ?", username)
            )
            if usercount != 0:
                return apology("Username already\nin use", code=400)

            # All checks have passed. Create user and log in.
            hash = generate_password_hash(password)
            session["user_id"] = db.execute(
                "INSERT INTO users (username, hash) VALUES(:username, :hash)",
                username=username,
                hash=hash,
            )
            return redirect("/")

        except:
            return apology("Invalid Input", code=400)

    # User reached route via GET (as by clicking a link or via redirect)
    else:
        return render_template("register.html")


@app.route("/sell", methods=["GET", "POST"])
@login_required
def sell():
    """Sell shares of stock"""
    # User reached route via POST (as by submitting a form via POST)
    if request.method == "POST":
        try:
            symbol = request.form["symbol"]
            shares = int(float(request.form["shares"]))
            if shares <= 0:
                return apology("Invalid Input", code=400)

            # Query database to see if user owns the stock
            rows = db.execute(
                "SELECT * FROM stocks WHERE user_id = :user_id AND symbol = :symbol",
                user_id=session["user_id"],
                symbol=symbol,
            )
            if len(rows) > 0:
                row = rows[0]
                if row["shares"] >= shares:
                    stock_quote = lookup(symbol)
                    price = stock_quote["price"]
                    extension = shares * price
                    profit = extension - ((row["total_paid"] / row["shares"]) * shares)

                    # Update database to increase cash
                    cash = db.execute(
                        "SELECT cash FROM users WHERE id = ?", session["user_id"]
                    )[0]["cash"]
                    db.execute(
                        "UPDATE users SET cash = :new_cash WHERE id = :id",
                        new_cash=cash + extension,
                        id=session["user_id"],
                    )

                    # Check to see if all shares are being sold
                    if row["shares"] == shares:
                        db.execute(
                            "DELETE FROM stocks WHERE user_id = :user_id AND symbol = :symbol",
                            user_id=session["user_id"],
                            symbol=symbol,
                        )
                    else:
                        set_shares = row["shares"] - shares
                        total_paid = row["total_paid"] - (
                            (row["total_paid"] / row["shares"]) * shares
                        )
                        db.execute(
                            "UPDATE stocks SET shares = :set_shares, total_paid = :total_paid WHERE user_id = :user_id AND symbol = :symbol",
                            user_id=session["user_id"],
                            symbol=symbol,
                            set_shares=set_shares,
                            total_paid=total_paid,
                        )

                        # Create history entry
                        db.execute(
                            "INSERT INTO history (user_id, symbol, shares, price, profit) VALUES(:user_id, :symbol, :shares, :price, :profit)",
                            user_id=session["user_id"],
                            symbol=symbol,
                            shares=-shares,
                            price=price,
                            profit=profit,
                        )

                    return generate_page(
                        "sell-message",
                        f"{shares:,} shares sold for ${extension:,.2f}. Profit: ${profit:,.2f}",
                    )

                else:
                    return apology("Invalid Input", code=400)
            else:
                return apology("Invalid Input", code=400)

        except:
            return apology("Invalid Input", code=400)

    # User reached route via GET (as by clicking a link or via redirect)
    else:
        return generate_page("index", None)
