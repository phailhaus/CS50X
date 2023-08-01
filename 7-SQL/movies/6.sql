SELECT AVG(rating) FROM movies LEFT JOIN ratings ON movies.id = ratings.movie_id WHERE year = 2012;
