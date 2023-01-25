import java.util.ArrayList;

public class PlaylistNodeSecondaryIndex extends PlaylistNode {
	private ArrayList<String> genres;
	private ArrayList<PlaylistNode> children;

	public PlaylistNodeSecondaryIndex(PlaylistNode parent) {
		super(parent);
		genres = new ArrayList<String>();
		children = new ArrayList<PlaylistNode>();
		this.type = PlaylistNodeType.Internal;
	}
	
	public PlaylistNodeSecondaryIndex(PlaylistNode parent, ArrayList<String> genres, ArrayList<PlaylistNode> children) {
		super(parent);
		this.genres = genres;
		this.children = children;
		this.type = PlaylistNodeType.Internal;
	}
	
	// GUI Methods - Do not modify
	public ArrayList<PlaylistNode> getAllChildren()
	{
		return this.children;
	}
	
	public PlaylistNode getChildrenAt(Integer index) {
		
		return this.children.get(index);
	}
	

	public Integer genreCount()
	{
		return this.genres.size();
	}
	
	public String genreAtIndex(Integer index) {
		if(index >= this.genreCount() || index < 0) {
			return "Not Valid Index!!!";
		}
		else {
			return this.genres.get(index);
		}
	}
	
	
	// Extra functions if needed
	public  Integer get_place(String genre) {
		int position = 0;
		while (position < this.genres.size() && genre.compareToIgnoreCase(this.genreAtIndex(position)) >= 0){
			position += 1;
		}
		return position;
	}

	public void set_child(int index, PlaylistNode child) {
		this.children.add(index, child);
	}
	public void set_value(int index, String value) {
		this.genres.add(index, value);
	}
	public  void update_child(int index, PlaylistNode child) {
		this.children.set(index, child);
	}

	public int get_position(String genre){
		int position = 0;
		for(int i = 0; i < this.genreCount(); i++){
			if(genre.compareToIgnoreCase(this.genreAtIndex(i)) < 0){
				return position;
			}
			else {
				position += 1;
			}
		}
		return position;
	}
}
