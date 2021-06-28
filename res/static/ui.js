let selected = null;

/**
 * Submit data or request to an endpoint on the server
 * @param endpoint http endpoint to access (ex.: /delete)
 * @param method 'POST', 'PUT', or 'GET'
 * @param data js object containing data to pass
 * @param success success callback
 * @param fail failure/error callback
 */
function ajax(endpoint, method, data, success, fail) {
    $.ajax({
        type: method,
        url: endpoint,
        data: JSON.stringify(data),
        contentType: 'application/json;charset=UTF-8',
        success: success,
        error: fail
    });
}

/**
 * Clear all child elements of a given element.
 * @param element The element to remove all children from.
 */
function clear_element_children(element) {
    while (element.firstChild) {
        element.removeChild(element.firstChild);
    }
}

class UI {
    constructor() {
        this._body = document.getElementById("main-card-body");
        console.log(this._body);
        this._active_nav = null;
        this.add_triangle();
    }

    _clear_previous() {
        clear_element_children(this._body);
        if (this._active_nav != null) {
            this._active_nav.classList.remove("active");
        }
    }

    get_form_data(form) {
        let un_indexed_array = form.serializeArray();
        let indexed_array = {};

        $.map(un_indexed_array, function (n, i) {
            indexed_array[n['name']] = n['value'];
        });

        return indexed_array;
    }

    _make_form_submittable(formName) {
        let form = $(formName);

        let self = this;
        form.submit(function (e) {
            ajax(form.attr('action'), form.attr('method'), self.get_form_data(form),
                function (data) {
                    app.log("POST " + formName + ": " + data['id']);
                    app.add_element(data);
                },
                function (data) {
                    app.log("POST error " + formName + ": " + data);
                });

            return false;
        });
    }

    add_triangle() {
        this._clear_previous();

        // todo: DOM
        this._body.innerHTML =
            '<form id="triangle-add" action="/add" method="post">\n' +
            '    <input type="hidden" name="type" value="triangle"/>\n' +
            '    <div class="form-group row">\n' +
            '        <label for="triangle-x" class="col-sm2 col-form-label">X:</label>\n' +
            '        <div class="col-sm-10">\n' +
            '            <input type="number" size=5 step=0.01 value=0 name="x" id="triangle-x" class="form-control"/>\n' +
            '         </div>\n' +
            '     </div>\n' +
            '     <div class="form-group row">\n' +
            '         <label for="triangle-y" class="col-sm2 col-form-label">Y:</label>\n' +
            '         <div class="col-sm-10">\n' +
            '             <input type="number" size=5 step=0.01 value=0 name="y" id="triangle-y" class="form-control"/>\n' +
            '         </div>\n' +
            '     </div>\n' +
            '     <div class="form-group row">\n' +
            '         <input type="submit" class="btn btn-outline-secondary" id="triangle-add-submit" value="Add"/>\n' +
            '     </div>\n' +
            '</form>';

        this._make_form_submittable('#triangle-add');

        this._active_nav = document.getElementById("nav-add-triangle");
        this._active_nav.classList.add("active")
    }

    add_text() {
        this._clear_previous();

        // todo: DOM
        this._body.innerHTML =
            '<form id="text-add" action="/add" method="post">\n' +
            '    <input type="hidden" name="type" value="text"/>\n' +
            '    <div class="form-group row">\n' +
            '        <label for="text-x" class="col-sm2 col-form-label">X:</label>\n' +
            '        <div class="col-sm-10">\n' +
            '            <input type="number" size=5 step=0.01 value=0 name="x" id="text-x" class="form-control"/>\n' +
            '        </div>\n' +
            '    </div>\n' +
            '    <div class="form-group row">\n' +
            '        <label for="text-y" class="col-sm2 col-form-label">Y:</label>\n' +
            '        <div class="col-sm-10">\n' +
            '            <input type="number" size=5 step=0.01 value=0 name="y" id="text-y" class="form-control"/>\n' +
            '        </div>\n' +
            '    </div>\n' +
            '    <div class="form-group row">\n' +
            '        <label for="text-text" class="sr-only col-sm2 col-form-label">Text content</label>\n' +
            '        <div class="col-sm-10">\n' +
            '            <input type="text" size=5 placeholder="Text" name="text" id="text-text" class="form-control"/>\n' +
            '        </div>\n' +
            '    </div>\n' +
            '    <div class="form-group row">\n' +
            '        <label for="text-wavy" class="col-sm2 col-form-label">Wavy</label>\n' +
            '        <div class="col-sm-10">\n' +
            '            <input type="checkbox" name="wavy" value="true" id="text-wavy" class="form-control"/>\n' +
            '        </div>\n' +
            '    </div>\n' +
            '    <div class="form-group row">\n' +
            '        <input type="submit" class="btn btn-outline-secondary" id="text-add-submit" value="Add"/>\n' +
            '    </div>\n' +
            '</form>';

        this._make_form_submittable('#text-add');

        this._active_nav = document.getElementById("nav-add-text");
        this._active_nav.classList.add("active");
    }
}

class App {
    constructor() {
        this.ui = new UI();
    }

    add_element(e) {
        // create new element
        let entry = document.createElement("div");
        entry.appendChild(document.createTextNode(e["type"] + e["id"]));

        // setup element
        entry.classList.add("log-line");
        entry.classList.add("list-group-item");
        entry.embedded = e;
        entry.onclick = function (event) {
            if (selected) {
                selected.classList.remove("selected");
            }
            selected = this;
            this.classList.add("selected");
        }
        entry.onclick(undefined);

        // append
        document.getElementById("selector").appendChild(entry);
    }

    delete_selected() {
        if (selected) {
            let self = this;
            ajax('/delete', 'PUT', {
                    type: '_delete',
                    id: selected.embedded["id"]
                },
                function (data) {
                    self.log('delete: ' + selected.embedded["id"]);
                    selected.parentNode.removeChild(selected);
                    selected = null;
                },
                function (data) {
                    self.log('delete error: ' + data)
                });
        }
    }

    log(text) {
        let textDiv = document.createElement("div");
        textDiv.appendChild(document.createTextNode(text));

        textDiv.classList.add("log-line");
        textDiv.classList.add("list-group-item");

        document.getElementById("commandLog").appendChild(textDiv);
    }
}

let app = null;

window.addEventListener("load", function () {
    app = new App();
});
