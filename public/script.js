(function(){
    const fields = {
        "genre-input":    "genre",
        "actor-input":    "actor",
        "director-input": "director"
    };

    for (let [inputId, category] of Object.entries(fields)) {
        const inp = document.getElementById(inputId);

        const listId = `${inputId}-list`;
        const datalist = document.createElement("datalist");
        datalist.id = listId;
        document.body.appendChild(datalist);

        inp.setAttribute("list", listId);

        inp.addEventListener("input", async () => {
            const prefix = inp.value.trim();
            if (!prefix) {
                datalist.innerHTML = "";
                return;
            }

            try {
                const resp = await fetch(
                    `/suggest?category=${category}&prefix=${encodeURIComponent(prefix)}`
                );
                if (!resp.ok) throw new Error("bad status");
                const arr = await resp.json();
                datalist.innerHTML = arr
                    .map(s => `<option value="${s}">`)
                    .join("");
            } catch {
                datalist.innerHTML = "";
            }
        });
    }

    document.getElementById("find-movie-button").addEventListener("click", async () => {
        const genre    = document.getElementById("genre-input").value.trim();
        const actor    = document.getElementById("actor-input").value.trim();
        const director = document.getElementById("director-input").value.trim();
        const useHeap  = document.getElementById("data-structure-toggle").checked;
        const order    = useHeap ? "heap" : "multimap";

        const params = new URLSearchParams();
        if (genre)    params.append("genre",    genre);
        if (actor)    params.append("actor",    actor);
        if (director) params.append("director", director);
        params.append("order", order);

        try {
            const resp = await fetch(`/search?${params}`, {
                headers: { "Accept": "application/json" }
            });
            if (!resp.ok) throw new Error(`Server returned ${resp.status}`);
            const data = await resp.json();
            showResults(data.movies || []);
        } catch (err) {
            console.error(err);
            document.getElementById("results").innerHTML =
                "<p>No movies found, or server error.</p>";
        }
    });

    function showResults(movies) {
        const container = document.getElementById("results");
        container.innerHTML = "";
        if (movies.length === 0) {
            container.innerHTML = "<p>No matching movies.</p>";
            return;
        }
        const ul = document.createElement("ul");
        for (let m of movies) {
            const li = document.createElement("li");
            li.textContent = `${m.title} – ${m.rating.toFixed(2)}`;
            ul.appendChild(li);
        }
        container.appendChild(ul);
    }
})();
