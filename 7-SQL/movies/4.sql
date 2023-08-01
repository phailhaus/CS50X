SELECT COUNT(title) FROM movies LEFT JOIN ratings ON movies.id = ratings.movie_id WHERE rating = 10;
