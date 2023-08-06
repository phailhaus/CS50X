var cost_scaler, score;

// Configure cost scaling
cost_scaler = 1.5;

score = 0;
const x = {
    value: 1,
    cost: 10,
    name: "X",
    id: "x"
};
const y = {
    value: 1,
    cost: 100,
    name: "Y",
    id: "y"
};
const z = {
    value: 1,
    cost: 1000,
    name: "Z",
    id: "z"
};

function score_click() {
    score_update(x.value * y.value * z.value);
}

function score_update(amount) {
    score += amount;
    document.getElementById("score_text").textContent = "Score = " + score.toFixed(0);
}

function component_buy(component) {
    if (component.cost <= score) {
        score_update(-component.cost);
        component.cost *= cost_scaler;
        component.value += 1;
        component_refresh(component);
    }
}

function component_refresh(component) {
    document.getElementById(component.id + "_text").textContent = component.name + " = " + component.value;
    document.getElementById(component.id + "_button").textContent = "Cost: " + component.cost.toFixed(0);
}